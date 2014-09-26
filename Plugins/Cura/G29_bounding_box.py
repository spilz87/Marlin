#Name: G29 Bounding Box 0.1
#Info: Find a print's bounding box and pass those coordinates to enhanced G29
#Help: G29BoundingBox
#Depend: GCode
#Type: postprocess
#Param: boundingBoxExtension(float:0) Buffer to add to print bounding box (mm)

## Prototype by Dave Beck, beck.dac@live.com
## Please contribute!
## Idea from various sources
## Please see the following forum topics:
## http://3dprintboard.com/showthread.php?6165-Marlin-firmware-fork-for-MakerFarm-i3v-w-auto-bed-level-amp-Roxy-s-enhancements
## http://3dprintboard.com/showthread.php?3105-Auto_Bed_Leveling-Enhanced-G29-command

## This script is licensed under the Creative Commons - Attribution - Share Alike (CC BY-SA) terms

# Uses -
# G29 V<verbosity flag, 0-4> T n<probe points in X & Y> L<in mm> F<in mm> R<in mm> B<in mm>
#    where LFRB define the bounding box coordinates in mm

#history / changelog:
# v 0.9: Initial prototype

version = '0.9'

import re
import sys # for maxint

def getValue(line, key, default = None):
	if not key in line or (';' in line and line.find(key) > line.find(';') and not ";LAYER:" in key):
		return default
	subPart = line[line.find(key) + len(key):] #allows for string lengths larger than 1
        if ";LAYER:" in key:
                m = re.search('^[+-]?[0-9]*', subPart)
        else:
                m = re.search('^[-]?[0-9]+\.?[0-9]*', subPart) #the minus at the beginning allows for negative values, e.g. for delta printers
	if m == None:
		return default
	try:
		return float(m.group(0))
	except:
		return default

bbL = -sys.maxint;
bbF = sys.maxint;
bbR = sys.maxint;
bbB = -sys.maxint;
with open(filename, "r") as f:
	lines = f.readlines()

x = 0;
y = 0;
layer = 0;
for line in lines:
	if getValue(line, 'G', None) == 1:
		x = getValue(line, 'X', None);
		y = getValue(line, 'Y', None);
		if x != None:
			if x > bbL:
				bbL = x;
			if x < bbR:
				bbR = x;
		if y != None:
			if y > bbB:
				bbB = y;
			if y < bbF:
				bbF = y;
		#print line
		#print repr(x) + " " + repr(y) + " - L" + repr(bbL) + " F" + repr(bbF) + " R" + repr(bbR) + " B" + repr(bbB)
	if getValue(line, 'G', None) == 29:
		G29line = line;
	if ';LAYER:' in line:
		layer = getValue(line, ';LAYER:', layer)
		if layer > 0:
			break;

# do error checking on bb values
# not implemented

# adjust bb values with boundingBoxExtension
# not implemented

print "Bounding Box: L" + repr(bbL) + " F" + repr(bbF) + " R" + repr(bbR) + " B" + repr(bbB)
print "G29 line before modification: " + G29line

# reprocess file and modify G29
# not implemented
