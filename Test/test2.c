#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

int main(){
    int a, b, fd_in, fd_in_2, ret;
    fd_set rset;
    

        //mkfifo("myfifo", 0666);
        fd_in=open("myfifo", O_RDONLY);
        //mkfifo("myfifo2", 0666);
        fd_in_2=open("myfifo2", O_RDONLY);
        
    while (1){
        
        FD_ZERO(&rset);
        FD_SET(fd_in, &rset);
        FD_SET(fd_in_2, &rset);
        ret=select(FD_SETSIZE, &rset, NULL, NULL, NULL);
        //printf("setsize: %d", FD_SETSIZE);
        //printf("wwww\n");
        if(ret==-1){
            perror("select()");
        }
        else if(ret>0){
            printf("Data is avaible, return value: %d \n", ret);
            fflush(stdout);
            if (FD_ISSET(fd_in, &rset)>0){
                read(fd_in, &a, sizeof(int));
                printf("a= %d\n", a);
            }
            if (FD_ISSET(fd_in_2, &rset)>0){
                read(fd_in_2, &b, sizeof(int));
                printf("b= %d\n", b);
            }

            
        }
        
    }
    printf("esce dal while\n");
}