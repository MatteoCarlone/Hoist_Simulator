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

/////////////////////////////

pid_t pid_motor_x;
pid_t pid_motor_z;
pid_t pid_command;

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and fancier, using errno.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

// Defining sighandler() function where we manage the reset tool
// through signals to make changes inside the while(1) loop.

void sighandler(int sig){
	if(sig==SIGUSR1){

		// When SIGUSR1 is called the alarm() function is reset.

		alarm(60);
	}
	if(sig==SIGALRM){  

		// When alarm() triggers, it sends SIGUSR2 signals to command, motor x 
		// and motor z to make them in reset state.

		CHECK(kill(pid_command, SIGUSR2));
		CHECK(kill(pid_motor_x, SIGUSR2));
		CHECK(kill(pid_motor_z, SIGUSR2));
		
	}
}

int main(int argc, char *argv[])
{	
	// declaring fd of FIFO command to watchdog and opening it.

	int fd_c_to_wd = CHECK(open(argv[1], O_RDONLY));

	// Saving the PIDs of the motor x and motor z process.

	pid_motor_x = atoi(argv[2]);
	pid_motor_z = atoi(argv[3]);

	// Declaring and building the use of the signals.

	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler=&sighandler;
	sa.sa_flags=SA_RESTART;

	// Calling for the first time alarm() function.

	alarm(60);
	sigaction(SIGUSR1,&sa,NULL);
	sigaction(SIGALRM,&sa,NULL);

	// Reading PID of the command process.

	CHECK(read(fd_c_to_wd, &pid_command, sizeof(int)));
	CHECK(close(fd_c_to_wd));
	
	// Looping endlessy the process without doing anything.

	while(1){

		sleep(1);

	}
}