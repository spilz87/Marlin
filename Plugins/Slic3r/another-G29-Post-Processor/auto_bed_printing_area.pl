#!/usr/bin/perl -i
#
# Post-processing script for the enhanced G29 gcode
# This script replaces the custom Gcode parameters [G29XMIN], [G29YMIN], [G29XMAX], [G29YMAX] by their values
# You have first to add custom gcode in your slicing app like << G29 L[G29XMIN] F[G29YMIN] R[G29XMAX] B[G29YMAX] n4 >>
# by Sylvain Catrix

use strict;
use warnings;
use File::Temp qw(tempdir);

my $temp_file = tempdir( CLEANUP => 1 )."/gcode.gcode"; #tempfile
my $x_max; #X min, overwrites LEFT_PROBE_BED_POSITION
my $x_min; #Y min, overwrites FRONT_PROBE_BED_POSITION
my $y_max; #X max, overwrites RIGHT_PROBE_BED_POSITION
my $y_min; #Y max, overwrites BACK_PROBE_BED_POSITION

#--------------
#read X/Y values from inputstream and store the current stream in a temporary file
#--------------
open(tmp_file,">".$temp_file) || die ("Cannot create a temporary file") ;
print "$temp_file\n";
while (<>) {
        if (/^(G1\sX(\d+(?:\.\d+)?)\sY(\d+(?:\.\d+)?)\s)/) {
                $x_max = $2 ;
                $x_min = $2 ;
                $y_max = $3 ;
                $y_min = $3 ;
                printf tmp_file $_;
                last();
        } else {
                printf tmp_file $_;
        }
}

while (<>) {
        if (/^(G1\sX(\d+(?:\.\d+)?)\sY(\d+(?:\.\d+)?)\s)/) {
                if ($2 > $x_max) {
                        $x_max = $2 ;
                }
                if($2 < $x_min) {
                        $x_min = $2 ;
                } 
                if ($3 > $y_max) {
                        $y_max = $3 ;
                }
                if($3 < $y_min) {
                        $y_min = $3 ;
                } 
                printf tmp_file $_;
        } elsif (/^(;\s+filament\s+used\s)/) {
                printf tmp_file $_;
                last();
        } else {
                printf tmp_file $_;
        }
}
close(tmp_file);

#--------------
#parametrise the G29 command from the temporary file and stream output gcode
#--------------
open(tmp_file,$temp_file) || die ("Cannot create a temporary file") ;
while (<tmp_file>) {
        if (/^G29/) { #G29 L[G29XMIN] F[G29YMIN] R[G29XMAX] B[G29YMAX] n4
                $_ =~ s/\[G29XMIN\]/$x_min/;
                $_ =~ s/\[G29YMIN\]/$y_min/;
                $_ =~ s/\[G29XMAX\]/$x_max/;
                $_ =~ s/\[G29YMAX\]/$y_max/;
                printf $_;
        } else {
                printf;
        }
}
close(tmp_file);

#--------------
#end the streams
#--------------
while (<>) {
                printf;
}

