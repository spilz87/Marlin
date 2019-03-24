// PHR
// Personal gcode send state vector
#include "../gcode.h"
#include "../../Marlin.h" // for pin_is_protected
#include "../../sd/cardreader.h"

extern int16_t feedrate_percentage;

extern unsigned long printtime;
extern unsigned long tempsEcouleGCODE;
extern uint32_t sdposLastTimeUpdate;
#include "../../module/printcounter.h"
#if ENABLED(PRINTCOUNTER)
  extern PrintCounter print_job_timer;
#else
  extern Stopwatch print_job_timer;
#endif

#if ENABLED(SDSUPPORT)
void GcodeSuite::P0() {
  unsigned long  ecoule = print_job_timer.duration();

  unsigned long  printTimeRest = 0;
  if(printtime != 0){
    printTimeRest = (printtime-tempsEcouleGCODE)*(10000-card.percentDone001(sdposLastTimeUpdate));
    printTimeRest /= feedrate_percentage;
    printTimeRest /= 100;
  }
        
  SERIAL_ECHO("P0:");
  SERIAL_ECHO(printtime); // temps total
  SERIAL_ECHO(":");
  SERIAL_ECHO(card.percentDone001()); // Pourcentage fait
  SERIAL_ECHO(":");
  SERIAL_ECHO(printTimeRest); // temps restant estime
  SERIAL_ECHO(":");
  SERIAL_ECHO(ecoule); // temps écoulé imprimante
  SERIAL_ECHO(":");
  card.printFilename(); // file name avec return
}
#endif
