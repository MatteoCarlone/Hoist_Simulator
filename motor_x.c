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
	int value;

	fd_c_to_mx = open(argv[1],O_RDONLY);
	if(fd_c_to_mx==-1){
		printf("There's an error opening the fifo.");
		fflush(stdout);
	}
	read(fd_c_to_mx, &value, sizeof(int));
	if (value==1){
		printf("Forza sampdoria");
		fflush(stdout);
		value=0;	
		}
	close(fd_c_to_mx);
}