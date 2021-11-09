#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(int argc, char * argv[]){
	int fd_c_to_mx;
	int value, ret;
	fd_set rset;
	fd_c_to_mx = open(argv[1],O_RDONLY);
	while(1){
		FD_ZERO(&rset);
        FD_SET(fd_c_to_mx, &rset);
        ret=select(FD_SETSIZE, &rset, NULL, NULL, NULL);

		if(ret==-1){
			printf("There's an error opening the fifo.");
			fflush(stdout);
		}
		else if(ret>0){
			if (FD_ISSET(fd_c_to_mx, &rset)>0){
				read(fd_c_to_mx, &value, sizeof(int));
				switch(value){

					case 1:
						printf("Freccetta verso destra!\n");
						//value=0;
					break;

					case 2:
						printf("Freccetta verso sinistra\n");
						//value=0;
					break;

					case 3:
						printf("Stop x axis!\n");
					break;
				}

			}
		}
	}	
	close(fd_c_to_mx);
}