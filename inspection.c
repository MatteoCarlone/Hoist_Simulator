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

	int fd_mx_to_ins;
	int ret;

	char c_1;

	float position;

	struct timeval tv={0,0};

	fd_set rset;

	// static struct termios oldt, newt;
    // tcgetattr( STDIN_FILENO, &oldt);
    // newt = oldt;
    // newt.c_lflag &= ~(ICANON);          
    // tcsetattr( STDIN_FILENO, TCSANOW, &newt);

	fd_mx_to_ins = open("fifo_est_pos_x",O_RDONLY);
	if (fd_mx_to_ins == -1){
		printf("Error opening mx to inspection fifo!");
		return 5;
	}

	while(1){

		FD_ZERO(&rset);
        FD_SET(fd_mx_to_ins, &rset);
        ret=select(FD_SETSIZE, &rset, NULL, NULL, &tv);

		if(ret==-1){
			printf("There's an error on select.");
			fflush(stdout);
		}
		else if(ret>=0){
			if (FD_ISSET(fd_mx_to_ins, &rset)>0){
				read(fd_mx_to_ins, &position, sizeof(float));
				printf("La posizione è: %f m\n", position);
			}	
		}
	}
	/* 	c_1=getchar();

	 	switch(c_1){
	 		case 115:
                printf("ho letto : %c", c_1);
            break;
		}
	}
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);*/
	return 0;
}
