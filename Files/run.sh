#!/bin/bash

# Running the program and moving the debug.txt from
# the executables folder to a new one dedicated to 
# the logfile.

cd $X
mkdir logfile
cd executables
./master
cd ..
mv executables/debug.txt logfile
