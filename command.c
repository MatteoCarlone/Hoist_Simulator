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
    int fd_c_to_mx;
    int p=1;

    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);


    while(1){

        c_1=getchar();

        switch(c_1){

            case 115:
                printf("ho letto : %c", c_1);
            break;

            case 27:

                c_2=getchar();
                c_3=getchar();

                switch(c_3){

                    case 65:
                    	printf("\nfreccetta_in_alto\n");
                        fd_c_to_mx=open("fifo_command_to_mot_x", O_WRONLY);
                        write(fd_c_to_mx, &p, sizeof(int));
                        close(fd_c_to_mx);
                    break;

                    case 66:
                    	printf("\nfreccetta_in_basso\n");
                    break;

                    case 67:
                    	printf("\nfreccetta_a_destra\n");
                    break;

                    case 68:
                    	printf("\nfreccetta_a_sinistra\n");
                    break;
                }
            break; 
            }
        }  
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}