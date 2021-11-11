#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <termios.h>

int main(void){   

    int c_1 , c_2 , c_3;
    int fd_c_to_mx, fd_c_to_mz;
    int right=1;
    int left=2;
    int xstop=3;
    int up = 4;
    int down = 5;
    int zstop = 6;

    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    
    fd_c_to_mx=open("fifo_command_to_mot_x", O_WRONLY);
    if (fd_c_to_mx==-1){
        printf("Error while trying to open the pipe");
    }
    fd_c_to_mz=open("fifo_command_to_mot_z", O_WRONLY);
    if (fd_c_to_mz == -1){
        printf("Error while trying to open the fifo");
    }

    while(1){

        c_1=getchar();

        switch(c_1){

            case 115:
                printf("ho letto : %c", c_1);
            break;

            case 120: //caso in cui premo x
                printf("ho letto : %c", c_1);
                write(fd_c_to_mx, &xstop, sizeof(int));
            break;

            case 122:
                printf("ho letto : %c", c_1);
                write(fd_c_to_mz, &zstop, sizeof(int));
            break;

            case 27:

                c_2=getchar();
                c_3=getchar();

                switch(c_3){

                    case 65:
                    	printf("\nfreccetta_in_alto\n");
                        write(fd_c_to_mz, &up, sizeof(int));
                    break;

                    case 66:
                    	printf("\nfreccetta_in_basso\n");
                        write(fd_c_to_mz, &down, sizeof(int));
                    break;

                    case 67:
                    	printf("\nfreccetta_a_destra\n");
                        write(fd_c_to_mx, &right, sizeof(int));
                    break;

                    case 68:
                    	printf("\nfreccetta_a_sinistra\n");
                        write(fd_c_to_mx, &left, sizeof(int));
                    break;
                }
            break; 
            }
        }
    close(fd_c_to_mx);
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}