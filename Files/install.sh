#!/bin/bash

# This is the install.sh file, you have to run it to install  the program.

# If there's no first argument the bash echoes the instruction to use pro-
# prierly the bash script. Remember to use source! So we can use the envi-
# roment of the shell we're running the .sh on.

if [ $# -eq 0 ]
then
 echo "Usage: source ./install.sh <pathname> ";
 echo "Pay attention to 'source' prefix!";
 exit;
fi

# The if statement is needed to check if the folder actually exists inside 
# our path. If it doesn't it asks the user if he wants to create it.

if [ ! -d $1 ]
then
 echo "Error: Directory $1 DOES NOT exist.";
 while true; do
  read -p "Do you want to create $1 directory? [Y/n] " yn
  case $yn in
   [Y]* ) mkdir $1; break;;
   [n]* ) exit;;
   * ) "Please, answer Y for yes or n for no.";;
  esac
 done
fi

# Now we actually begin the real installation of the program by unzipping 
# the src.zip. We also export the <path> of the folder to use it in anot-
# her file bash.

echo "Begin program installation on $1 ... ";
export X=$1
unzip src.zip -d $1;

# Now we compile all the .c files and we put the executables inside a new 
# folder called executables.

echo "Begin sources' compilation ...";

mkdir $1/executables;
gcc $1/dmaster/master.c -o $1/executables/master;
gcc $1/dcommand/command.c -o $1/executables/command;
gcc $1/dmotor_x/motor_x.c -o $1/executables/motor_x;
gcc $1/dmotor_z/motor_z.c -o $1/executables/motor_z;
gcc $1/dinspection/inspection.c -o $1/executables/inspection;
gcc $1/dwatchdog/watchdog.c -o $1/executables/watchdog;

echo "You can run the program in $1 with ./run.sh ";

