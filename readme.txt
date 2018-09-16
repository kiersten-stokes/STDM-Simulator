Kiersten Stokes 
==================

+--------------------------+
| BUILD & RUN INSTRUCTIONS |
+--------------------------+

STDM.c
-------------------
Compile:	make
Usage:		./STDM [inputfile.txt]


+-------+
| NOTES |
+-------+

This program works as tested on Ubuntu 14.02
and nunki.usc.edu.

This program assumes the correct input file
format and does not do error checking for
integer inputs, monotonically increasing 
timestamps, same data rate for each source, 
etc. It also assumes the input file has at
least one source with at least one frame.

This program assumes a time difference (e.g.
starttime - endtime) of 1 time unit, as dis-
cussed in the class discussion group.

This program hardcodes the limit on the add-
ress size at 8 bits, which in turn limits 
the number of possible data sources to 256.
If desired, please update the #define state-
ment in STDM.h (line 8). In the case of only
one or two sources, the address will still 
be two bits in length.

This program hardcodes the limit on the num-
ber of timeslots that it can handle at 100.
If the test source file has a timeslot 
greater than 100, please update the #define 
statement in STDM.h (line 9).

This program outputs to stdout. See example
of first few lines of output in box below.
It prints the output link rate in terms of
(subframes/slot)*R + (overhead bits). It 
then prints a list of frames transmitted.
A start flag (*) and end flag (*) appear at
each end. The source address (in binary) 
preceeds the frame number being transmitted
for each of the subframes in the frame.
Finally, the frame transmission time start
and end are printed to the right of the 
frame diagram. Empty frames are printed as
a series of dashes rather than an address
and frame number. Backlogged frames are
sent as soon as possible, but not called 
out within the frame diagram specifically.

+---------------------------------------------------------------------------------------+
|	output link rate: 2R+6
|
|	| * | 00 | A1 | 01 | B1 | * |		(frame start time: 0, end time: 1)
|	| * | 00 | A2 | 01 | B2 | * |		(frame start time: 1, end time: 2)
|
|	...
|
|	| * | 11 | D1 | -- | -- | * |		(frame start time: 4, end time: 5)
|
|	... etc.
+---------------------------------------------------------------------------------------+

