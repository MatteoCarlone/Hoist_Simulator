#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(){

    int a, b, fd_out, fd_out_2;
    pid_t child=fork();
    if (child<0){
        perror("error forking");
        return 1;
    }
    if(child == 0){
        char * arg_list[] = {"./test2", NULL, NULL};
        execvp("./test2", arg_list);

    }
    else{
        mkfifo("myfifo", 0666);
        mkfifo("myfifo2", 0666);
        fd_out=open("myfifo", O_WRONLY);
        fd_out_2=open("myfifo2", O_WRONLY);

    while(1){    
        printf("inserisci: \n");
        scanf("%d", &a);
        scanf("%d", &b);

        write(fd_out, &a, sizeof(int));

        write(fd_out_2, &b, sizeof(int));
        sleep(1);
    }
    }
//close(fd_in);
//close(fd_in_2);
wait(child);
return 0;

}