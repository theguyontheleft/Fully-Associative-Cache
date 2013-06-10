Fully-Associative-Cache
=======================

James Dagres

A Fully Associative Cache and Computer Simulator

DESCRIPTION:
=============

This software uses a GUI interface to run a computer simulator than runs a fully associative cache. It works by opening any text document containing instructions.

FEATURES
=======================================
* Can simulate 9 different Cache options
* Has configurable cache size of 16, 32 or 64 words of memory
* Has configurable cache line of 2, 4 or 8 words of memory
* Can handle any Instruction Set Architecture (9 different instructions)
* Can run instructions line by line
* Can run all instructions
* Has Breakpoint debugging
The breakpoint feature is an easy way to jump to any desired instruction. Simply set the breakline
and click run and the program will stop/break after executing every instruction up to the instruction
on the breakline. The instruction on the breakline is not executed when the program breaks.
Afterward reaching the first breakpoint a second breakpoint can made.

NOTES
=======================================
Originally I was told to implement a direct cache, but seeing as  
that didn't seem very useful and didn't make as much sense to me
I took it a step further and developed a fully associative cache. 

ADDITIONAL PROGRAMS TO RUN THE PROJECT:
=======================================
QT
* The program can be made and executed from the terminal as well 
as from QT.
For references on how to install Qt on various OS, please go to
http://qt-project.org/doc/qt-4.8/installation.html
* To install QT on a Linux OS the following terminal instruction 
will download everything needed.
Be warned it is a massive download (174 MB):

        $ sudo apt-get install qt-sdk

INSTRUCTIONS:
=======================================
Building the project
* From the terminal once in the project directory run the 
following commands to make and build the project:

        $ qmake -project
        $ qmake
        $ make
        
* There are some test files to try the project in the TestFiles 
directory.
* To run the program enter the command

        $ ./ComputerSim

* You can open any of those file with the open button pinned to 
the quick access toolbar (Hover over the "ComputerSimGUI" menu and a "file" option will appear,
from the file option a file can opened and then later saved.
* Once the file has been loaded the GUI's interface should be 
pretty straight forward.

Using the Breakpoint
* There is an enable Breakpoint button that toggles the breakline spinbox on and off.
* The breakline spinbox is initialized to zero. 
  
   a) A valid breakline can be any number from 1-127
  	    note: 128 is the max number of instructions, and you can only step to the first (0th) instruction.

   b) A valid breakline will be highlighted yellow.

* There can only be one breakpoint at a time, if a second breakpoint is desired it can be set after
   the first breakpoint is reached

* The breakpoint feature is disabled and reset when:

   a) The breakline is reached.
   
   b) The program clears or resets.
   
   c) The breakpoint button is toggled.

* When the breakpoint feature is disabled: 

   a) The spinbox is not selectable and is defaulted to zero.
   
   b) The selected breakline is unhighlighted.
