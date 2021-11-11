#include<stdio.h>
#include <termios.h>              
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char * argv[]){

	int fd_mx_to_ins , fd_mz_to_ins;
	int ret;
	int pid_motor_x, pid_motor_z;
	char c_1;

	float position_x , position_z;

	struct timeval tv={0,0};

	fd_set rset;
 
	static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

	fd_mx_to_ins = open("fifo_est_pos_x",O_RDONLY);
	if (fd_mx_to_ins == -1){
		printf("Error opening mx to inspection fifo!");
		return 5;
	}

	fd_mz_to_ins = open("fifo_est_pos_z",O_RDONLY);
	if (fd_mz_to_ins == -1){
		printf("Error opening mx to inspection fifo!");
		return 5;
	}

	pid_motor_x = atoi(argv[1]);
	pid_motor_z = atoi(argv[2]);

	while(1){

		FD_ZERO(&rset);
        FD_SET(fd_mx_to_ins, &rset);
        FD_SET(fd_mz_to_ins, &rset);
        FD_SET(0,&rset);
        ret=select(FD_SETSIZE, &rset, NULL, NULL, &tv);

		if(ret==-1){
			printf("There's an error on select.");
			fflush(stdout);
		}
		else if(ret>0){
			if (FD_ISSET(fd_mx_to_ins, &rset)>0 && FD_ISSET(fd_mz_to_ins, &rset)>0){
				read(fd_mx_to_ins, &position_x, sizeof(float));
			}
			if (FD_ISSET(0,&rset)>0){
				read(0, &c_1, sizeof(char));
				if(c_1=='s'){
					kill(pid_motor_x, SIGUSR1);
					kill(pid_motor_z, SIGUSR1);
				}
				if(c_1=='r'){
					kill(pid_motor_x,SIGUSR2);
					kill(pid_motor_z,SIGUSR2);
				}
			}

			if (FD_ISSET(fd_mz_to_ins, &rset)>0){
				read(fd_mz_to_ins, &position_z, sizeof(float));
			}
		}
		printf("\rLa posizione lungo x è: %f m, La posizione lungo z è: %f m", position_x, position_z);
		fflush(stdout);
	}
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	return 0;
}
