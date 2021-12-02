//// Including Libraries ////

#include <stdio.h>
#include <termios.h>              
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

/////////////////////////////

//// Defining Colors ////

#define BHRED "\e[1;91m"
#define BHWHT "\e[1;97m"
#define BHYEL "\e[1;93m"
#define BHMGN "\033[1m\033[35m"
#define RESET "\033[0m"

/////////////////////////

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and more fancy, using errno. As far as the stderr is never
// going to be printed on the konsole. We decided to print it out on the logfile.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(out,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

// Declaring the variable to save the process id of the command console.

pid_t pid_command;

int main(int argc, char * argv[]){

	// Declaring local integer variables, they are grouped to get a better  
    // look of the code and making it easier to modify or fix.

	int wait = 0;
	int fd_mx_to_ins , fd_mz_to_ins, fd_command_to_ins; 
	int position_x , position_z;
	int ret;

	// Declaring variables to save the processes id of the two motors and of the watchdog.

	pid_t pid_motor_x, pid_motor_z, pid_wd;

	// Declaring a char type variable that we will use for the input of the emergency commands (STOP and RESET).

	char c_1;

	// Declaring time variables.

	time_t current_time;
	struct timeval tv={0,0};

	// Declaring the rset as the file descriptor set, where we put the file descriptors of those
	// files we want to read.

	fd_set rset;

	// Opening the Log-File in append_mode.

	FILE *out = fopen("debug.txt", "a");
 	if(out == NULL){
        printf("ERRROR OPEN FILE");
    }
    fprintf(out, "PID ./inspection: %d\n", getpid());

    // declaring the two termios structs.

	static struct termios oldt, newt;

	// This is a method to not use the enter button on the two consoles.
    // The tcgetattr() function shall get the parameters associated with 
    // the terminal referred to by the first argument and store them in 
    // the termios structure referenced by the second argument.
    // The first argument is an open file descriptor associated with a 
    // terminal. STDIN_FILENO is the default standard input fd (0).

    CHECK(tcgetattr( STDIN_FILENO, &oldt));
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          				// This makes the standard input taking values  
    CHECK(tcsetattr( STDIN_FILENO, TCSANOW, &newt));	// without waiting for enter to be pressed.

    // Now we want to open the fifos and use them to make the inspection process
    // communicating with the other processes.

    fd_command_to_ins = CHECK(open(argv[3], O_RDONLY));
	fd_mx_to_ins = CHECK(open(argv[4],O_RDONLY));
	fd_mz_to_ins = CHECK(open(argv[5],O_RDONLY));

	// Changing the type of the PIDs from string to int.

	pid_motor_x = atoi(argv[1]);
	pid_motor_z = atoi(argv[2]);
	pid_wd = atoi(argv[6]);

	printf(BHMGN "\n" " ######################################################################################" RESET "\n");
	printf(BHMGN " #  _ _____ _____ _____ _____ _____ _____ _ _____ _____    _                   _      #" RESET "\n");
	printf(BHMGN " # |_|   | |   __|  _  |   __|     |_   _|_|     |   | |  | |_ ___ ___ ___ ___| |___  #" RESET "\n");
	printf(BHMGN " # | | | | |__   |   __|   __|   --| | | | |  |  | | | |  | '_| . |   |_ -| . | | -_| #" RESET "\n");
	printf(BHMGN " # |_|_|___|_____|__|  |_____|_____| |_| |_|_____|_|___|  |_,_|___|_|_|___|___|_|___| #" RESET "\n");
	printf(BHMGN " #                                                                                    #" RESET "\n");
	printf(BHMGN " ######################################################################################" RESET "\n\n");

	// Reading the PID of the command console.

	CHECK(read(fd_command_to_ins,&pid_command, sizeof(int)));

	// Closing the file descriptor.

	CHECK(close(fd_command_to_ins));

	while(1){

		// Opening fifos in read mode

		fd_mx_to_ins = CHECK(open(argv[4],O_RDONLY));
		fd_mz_to_ins = CHECK(open(argv[5],O_RDONLY));

		// Implementing the part of the code spent to use select() function, we want to read FIFO's 
		// when they are ready.

		FD_ZERO(&rset);
        FD_SET(fd_mx_to_ins, &rset);
        FD_SET(fd_mz_to_ins, &rset);
        FD_SET(0,&rset);
        ret = CHECK(select(FD_SETSIZE, &rset, NULL, NULL, &tv));

        // Saving the current time.

        time(&current_time);

        // Now we want to read the file descriptors if and only if they are ready.

		if(ret>0){

			// Reading position infos from motor_x and motor_z.

			if (FD_ISSET(fd_mx_to_ins, &rset)>0){
				CHECK(read(fd_mx_to_ins, &position_x, sizeof(int)));
			}

			if (FD_ISSET(fd_mz_to_ins, &rset)>0){
				CHECK(read(fd_mz_to_ins, &position_z, sizeof(int)));
			}

			if (FD_ISSET(0,&rset)>0){

				read(0, &c_1, sizeof(char));

				// Switch-case to manage the user's input for the emergency commands (STOP and RESET).
				switch(c_1){

					case 's':

					printf("\n"BHRED "  EMERGENCY STOP" RESET "\n");
					fflush(stdout);

					// Sending the Signals to Stop the motors and rehabilitate the command console
					// after a Reset.
					// Furthermore we alarm the watchdog.

					CHECK(kill(pid_command, SIGUSR1));
					CHECK(kill(pid_motor_x, SIGUSR1));
					CHECK(kill(pid_motor_z, SIGUSR1));
					CHECK(kill(pid_wd,SIGUSR1));
					
					// printing on the Log-File.

					fprintf(out, "Stop button pressed. ");
					fprintf(out, "                    Time:  %s", ctime(&current_time));

					break;
				
					case 'r':

					printf("\n"BHYEL "  RESET" RESET "\n");
					fflush(stdout);

					// Sending the Signals to Reset the motors and disable the command console
					// Furthermore we alarm the watchdog.

					CHECK(kill(pid_command,SIGUSR2));
					CHECK(kill(pid_motor_x,SIGUSR2));
					CHECK(kill(pid_motor_z,SIGUSR2));
					CHECK(kill(pid_wd,SIGUSR1));
					
					fprintf(out, "Reset button pressed.");
					fprintf(out, "                    Time:  %s", ctime(&current_time));

					break;


                    default:

                   		// Managing wrong inputs.

                        printf("\n" BHRED "  Command Not Allowed" RESET "\n");
                        fflush(stdout);

                    break;

				}
			}
		}

		if(position_x==0 && position_z==0){

			// Signal to rehabilitate the command console after the reset finish.

			kill(pid_command, SIGUSR1);
		}

		// Closing the file descriptors.

		CHECK(close(fd_mx_to_ins));
		CHECK(close(fd_mz_to_ins));

		printf(BHWHT "\r  La posizione lungo x è: %d m, La posizione lungo z è: %d m" RESET, position_x, position_z);
		fflush(stdout);
  		fflush(out);

  		// Printing on the Log-File once every 1000 cicles.
  		
  		if(wait%1000==0) fprintf(out, "Position x: %d, Position z: %d     Time:  %s", position_x, position_z, ctime(&current_time));
  		wait++;
	}

	return 0;
}
