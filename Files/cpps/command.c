// Including libraries.

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
#include <errno.h>
#include <time.h>


// Defining colors of the lines in the console.

#define BHRED "\e[1;91m"       // Bold Red. ..
#define BHGRN "\e[1;92m"     // Bold Green. ..
#define BHYEL "\e[1;93m"    // Bold Yellow. ..
#define BHBLU "\e[1;94m"      // Bold Blue. ..
#define BHMAG "\e[1;95m"   // Bold Magenta. ..
#define BHCYN "\e[1;96m"      // Bold Cyan. ..
#define BHWHT "\e[1;97m"     // Bold White. ..
#define RESET "\033[0m"

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and more fancy, using errno. As far as the stderr is never
// going to be printed on the konsole. We decided to print it out on the logfile.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(out,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

// We decided to create the type bool to make the code easier to
// understand, as long as the bool type is really useful.

typedef enum {
    false,
    true
}bool;

// Declaring the variable reset, we will see later in the code why it is
// so important.

bool reset = false;

// Declaring sighandler() function, this function will be called whenever
// an emergency button has been pressed in the inspection konsole.
// SIGUSR2 is used to start the reset routine of the position of both axis.
// SIGUSR1 is used to stop the reset routine.

void sighandler(int sig){

    if(sig==SIGUSR2){

        // Sistem resetting, disabling commands.

        reset = true;
        printf("\n" BHBLU "  SYSTEM RESETTING" RESET "\n");      // BOLDBLUE, BOLDYELLOW, RESET
        fflush(stdout);                                             // are macros to color the text
        printf("\n" BHYEL "  COMMAND DISABLED" RESET "\n");    // on the consoles.
        fflush(stdout);
    }

    if(sig==SIGUSR1) {

        // Sistem stop, commands rehabilitated.

        if(reset==true){
            reset = false;
            printf("\n" BHYEL "  COMMAND REHABILITATED" RESET "\n");
        }
        else{
            reset = false;
        }
    }
}

// Creating main() function, where all the main tasks will take place.

int main(int argc, char *argv[]){   

    // Opening and checking errors the debug file in "a" mode, which means 
    // append. This parameter is fundamental because if it wasn't existing 
    // it wouldn't there be the other lines written in the other processes.  
    // We would have the problem of overwriting the file.

    FILE *out = fopen("debug.txt", "a");
    if(out == NULL){
        printf("ERRROR OPEN FILE");
    }

    // Printing PID.

    fprintf(out, "PID ./command: %d\n", getpid()); fflush(out);

    // Time declaring.
    
    time_t current_time;
    struct timeval tv={0,0};

    // Declaring local integer variables, they are grouped to get a better  
    // look of the code and making it easier to modify or fix.

    int c_1 , c_2 , c_3;
    int fd_c_to_mx, fd_c_to_mz, fd_c_to_ins, fd_c_to_wd;
    int right = 1, left = 2, xstop=3, up = 4, down = 5, zstop = 6;

    // Declaring and assigning values of the PIDs of the command and the 
    // inspection consoles.

    pid_t pid_command = getpid();
    pid_t pid_wd = atoi(argv[1]);

    // declaring the two termios structs.

    static struct termios oldt, newt;

    // Declaring and building the use of the signals.

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler=&sighandler;
    sa.sa_flags=SA_RESTART;
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGUSR1, &sa,NULL);

    // This is a method to not use the enter button on the two consoles.
    // The tcgetattr() function shall get the parameters associated with 
    // the terminal referred to by the first argument and store them in 
    // the termios structure referenced by the second argument.
    // The first argument is an open file descriptor associated with a 
    // terminal. STDIN_FILENO is the default standard input fd (0).

    CHECK(tcgetattr( STDIN_FILENO, &oldt));
    newt = oldt;
    newt.c_lflag &= ~(ICANON);                              // This makes the standard input taking values     
    CHECK(tcsetattr( STDIN_FILENO, TCSANOW, &newt));        // without waiting for enter to be pressed.

    // Now we want to open the fifos and use them to make command process
    // communicating with the other processes.

    fd_c_to_ins = CHECK(open(argv[2] ,O_WRONLY));
    fd_c_to_mx = CHECK(open(argv[3], O_WRONLY));
    fd_c_to_mz = CHECK(open(argv[4], O_WRONLY));
    fd_c_to_wd = CHECK(open(argv[5], O_WRONLY));

    // Printing on the console informations about the simulator.

    printf("\n" BHMAG " ############################################################################" RESET "\n");
    printf(BHMAG " #  _____ _____ _____ _____ _____ _____ ____     _                   _      #\n");
    printf(BHMAG " # |     |     |     |     |  _  |   | |    |   | |_ ___ ___ ___ ___| |___  #\n");
    printf(BHMAG " # |   --|  |  | | | | | | |     | | | |  |  |  | '_| . |   |_ -| . | | -_| #" RESET "\n");
    printf(BHMAG " # |_____|_____|_|_|_|_|_|_|__|__|_|___|____/   |_,_|___|_|_|___|___|_|___| #" RESET "\n");
    printf(BHMAG " #                                                                          #" RESET "\n");
    printf(BHMAG " ############################################################################" RESET "\n\n");

    printf(BHRED "  Welcome to you my friend, this is a simulator of a hoist robot!" RESET);    
    printf("\n" BHRED "  Created by Matteo Carlone and Luca Predieri." RESET "\n\n");    
    printf(BHYEL "  Here there's a list of commands:" RESET "\n");
    printf(BHGRN "  If you want to move, press right arrow!" RESET "\n");
    printf(BHCYN "  If you want to move back, press left arrow!" RESET "\n");
    printf(BHBLU "  If you want to move down, press up arrow!" RESET "\n");
    printf(BHBLU "  If you want to move up, press down arrow!" RESET "\n");
    printf(BHWHT"  To stop the movement of the two axis, you can press X or Z!" RESET "\n\n");

    // Passing the command PID to inspection and watchdog process.

    CHECK(write(fd_c_to_ins, &pid_command, sizeof(int)));
    CHECK(write(fd_c_to_wd, &pid_command, sizeof(int)));
    CHECK(close(fd_c_to_ins));
    CHECK(close(fd_c_to_wd));

    // Now we are going to study the loop part of command process.

    while(c_1 = getchar()){

        // Time stuff.

        time(&current_time);
    
        // Reading the input on the command console.

        switch(reset){

            // When the process is hit by SIGUSR2 the command can't take no
            // more inputs.

            case true:

                if(c_1 != 0){
                    printf("\n" BHRED " SYSTEM RESETTING, WAIT UNTIL IT FINISHES!" RESET "\n");
                    fflush(stdout);
                }

                if(c_1 == 32){
                    exit(1);
                }

            break;

            // When the process is hit by SIGUSR1 the command process is set
            // to the default tasks.

            case false:

                switch(c_1){

                    // Case when 'x' button is pressed on the keyboard, the x axis does
                    // not increase no more. It just stops by sending a signal to the
                    // motor x process.

                    case 120:

                        printf("\n" BHRED "  X Axis Stopped" RESET "\n");
                        fflush(stdout);

                        CHECK(write(fd_c_to_mx, &xstop, sizeof(int)));
                        CHECK(kill(pid_wd, SIGUSR1));

                        fprintf(out, "Pressed x, x axis stopped.               Time:  %s", ctime(&current_time));
                        fflush(out);

                    break;

                    // Case when 'z' button is pressed on the keyboard, the z axis does
                    // not increase no more. It just stops by sending a signal to the
                    // motor z process.

                    case 122:

                        printf("\n" BHRED "  Z Axis Stopped" RESET "\n");
                        fflush(stdout);

                        CHECK(write(fd_c_to_mz, &zstop, sizeof(int)));
                        CHECK(kill(pid_wd, SIGUSR1));

                        fprintf(out, "Pressed z, z axis stopped.               Time:  %s", ctime(&current_time));
                        fflush(out);
    
                    break;

                    case 27:

                        // Reading the other 

                        c_2=getchar();
                        c_3=getchar();

                        switch(c_3){

                            // UP Arrow, in ASCII code three different inputs 27, 91, 65. We want to pass
                            // to the motor z process a code to start the motor of the z axis. We pass a 
                            // signal to the watchdog to make the alarm() function restart.

                            case 65:

                            	printf("\n" BHGRN "  UP Arrow" RESET "\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mz, &up, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed up arrow, z axis increasing.     Time:  %s", ctime(&current_time));
                                fflush(out);

                            break;

                            // DOWN Arrow, in ASCII code three different inputs 27, 91, 66. We want to pass
                            // to the motor z process a code to start reversed the motor of the z axis. We 
                            // pass a signal to the watchdog to make the alarm() function restart. 

                            case 66:

                            	printf("\n" BHGRN"  DOWN Arrow" RESET "\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mz, &down, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed down arrow, z axis decreasing.   Time:  %s", ctime(&current_time));
                                fflush(out);
                                

                            break;

                            // RIGHT Arrow, in ASCII code three different inputs 27, 91, 67. We want to pass
                            // to the motor x process a code to start the motor of the x axis. We 
                            // pass a signal to the watchdog to make the alarm() function restart. 

                            case 67:

                            	printf("\n" BHGRN "  RIGHT Arrow" RESET "\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mx, &right, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed right arrow, x axis increasing.  Time:  %s", ctime(&current_time));
                                fflush(out);
                  
                            break;

                            // LEFT Arrow, in ASCII code three different inputs 27, 91, 68. We want to pass
                            // to the motor x process a code to start reversed the motor of the x axis. We 
                            // pass a signal to the watchdog to make the alarm() function restart. 

                            case 68:

                            	printf("\n"BHGRN"  LEFT ARROW"RESET"\n");
                                fflush(stdout);

                                CHECK(write(fd_c_to_mx, &left, sizeof(int)));
                                CHECK(kill(pid_wd, SIGUSR1));

                                fprintf(out, "Pressed left arrow, z axis decreasing.   Time:  %s", ctime(&current_time));
                                fflush(out);
                            
                            break;

                        }

                    break; 


                    case 32:
                        CHECK(close(fd_c_to_mx));
                        CHECK(close(fd_c_to_mz));
                        exit(1);
                    break;

                    // If none of the right keys are pressed, we let the user know
                    // that he's not pressing the right keys.

                    default:

                        printf("\n"BHRED "  Command Not Allowed" RESET "\n");
                        fflush(stdout);

                    break;
                }

            break;
        }

    }
}
