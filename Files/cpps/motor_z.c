//// Including Libraries ////

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

////////////////////////////

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and fancier, using errno.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

// We decided to create the type bool to make the code easier to
// understand, as long as the bool type is really useful.

typedef enum {
	false,
	true
}bool;

// Declaring variables.

bool reset = false;
int value;
int position=0;
int step=1;

// Defining sighandler() function where we manage the reset tool
// through signals to make changes inside the while(1) loop.

void sighandler(int sig){
	if(sig==SIGUSR1){
		reset = false;
		value=6;
	}
	if(sig==SIGUSR2){

		reset = true;
	}
}

// Creating main() function, where all the main tasks will take place.

int main(int argc, char * argv[]){

	// Declaring local variables.

	int fd_c_to_mz, fd_mz_to_ins;
	int ret;
	int err;
	struct timeval tv={0,0};

	// Declaring and building the use of the signals.

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler=&sighandler;
	sa.sa_flags=SA_RESTART;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);

	// Declaring rset as the file descriptor set where we put the file descriptors of those
	// files we want to read.

	fd_set rset;

	while(1){

		// Opening FIFOs.

		fd_c_to_mz = CHECK(open(argv[1],O_RDONLY));
		fd_mz_to_ins = CHECK(open(argv[2], O_WRONLY));

		// Creating error.

		err = (rand()%(1)) - (rand()%(1));

		// Implementing the part of the code spent to use select() function, we want to read the FIFOs
		// when they are ready. 

		FD_ZERO(&rset);
        FD_SET(fd_c_to_mz, &rset);
        ret = select(FD_SETSIZE, &rset, NULL, NULL, &tv);

        // If the FIFO is ready, we read it.

		if(ret>=0){
			if (FD_ISSET(fd_c_to_mz, &rset)>0){
				CHECK(read(fd_c_to_mz, &value, sizeof(int)));
			}
		}

		// Managing the reset tool, we want to make two distinct cases because when we use the reset
		// we want the machine to go to the real 0, where there are no errors.

		switch(reset){

			// Reset is running.

			case true:

				// If the position is higher than the step we want to decrement the value of the position.

				if(position>step){
					position -= step+err;
					usleep(10000);
					CHECK(write(fd_mz_to_ins, &position, sizeof(int)));		
				}

				// If the postion is lower than the step we want the position in its origin.

				if (position<=step){
					position=0;
					value = 6;
					reset = false;
				}

			break;

			// Reset is not running.

			case false:

				switch(value){

					// We want to increase the position.

					case 4:
						if (position>=6000){
						}
						else{
							position+= step+err;
						}
						usleep(10000);

					break;

					// We want to decrease the position.

					case 5:

						if (position<=0){
						}
						else {
							position-=step+err;
						}
						usleep(10000);

					break;

					// If the robot is stopped we want it to stop.

					case 6:
						usleep(10000);
						//value = 0;
					break;
				}

			if (position>6000) position=6000;
			if (position<0) position=0;
			
			// Writing the postition to the inspection console.

			CHECK(write(fd_mz_to_ins, &position, sizeof(int)));		

			break;
		}
	
	// Closing all the file descriptors.

	CHECK(close(fd_mz_to_ins));
	CHECK(close(fd_c_to_mz));			  
	}
}