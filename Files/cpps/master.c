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
#include <sys/wait.h>

// Defining CHECK() tool. We use this error checking method to make the
// code lighter and fancier, using errno.

#define CHECK(X) ({int __val = (X); (__val == -1 ? ({fprintf(stderr,"ERROR (" __FILE__ ":%d) -- %s\n",__LINE__,strerror(errno)); exit(-1);-1;}) : __val); })

// Declaring status of the first child process dead.

int status;

// Declaring process IDs and the relatives strings.

pid_t pid_command, pid_motor_x, pid_motor_z, pid_inspection, pid_wd;
char pid_motor_x_a[20], pid_motor_z_a[20], pid_command_a[20], pid_wd_a[20];

// Declaring and creating spawn() function, to make the code easier to 
// understand and more flowing.

int spawn(const char * program, char ** arg_list) {

  pid_t child_pid = fork();
  if (child_pid != 0)
    return child_pid;

  else {
    execvp (program, arg_list);
    perror("exec failed");
    return -1;
  }

}

// Declaring and creating create_fifo() with the same purpose of spawn()
// function, we want a more readable code so we can fix possible errors.

void create_fifo (const char * name){ 

    if(mkfifo(name, 0666)==-1){
        if (errno != EEXIST){
          perror("Error creating named fifo\n");
          exit(-1);
        }
    }

}

// Creating main() function, where all the main tasks will take place.

int main() {

  // Printing some useful informations.

  printf("\nWelcome to you my friend, ML99 hoist robot simulator!\n");
  printf("If you want to terminate the program press the space bar on the command console!\n\n"); fflush(stdout);
  printf("Process errors displayed in the terminal:\n");
  printf("./master\n");
  printf("./motor_x\n");
  printf("./motor_z\n");
  printf("./watchdog\n");
  printf("Process errors displayed on the logfile:\n");
  printf("./command\n");
  printf("./inspection\n\n"); fflush(stdout);

  // Getting PID of ./master.

  pid_t pid_master = getpid();

  // Creating and opening the debug file, where all the movement of the hoist
  // robot will be recorded. We can say it is a sort of machine data recorder
  // like all the modern machines (like planes) must have.

  FILE *out = fopen("debug.txt", "w");
  fprintf(out, "################################## DEBUG FILE ################################## \n\n");
  fflush(out);
  CHECK(fclose(out));

  // Now we declare the name and the location (a tmp folder in the project
  // folder) of the fifos that we're going to use throughout all the code.
  // They're going to be six, each name contains the name of the process 
  // between which there will be the communication.

  char * f_com_to_mx = "/tmp/fifo_command_to_mot_x";    // Command and motor x.
  char * f_com_to_mz = "/tmp/fifo_command_to_mot_z";    // Command and motor z.
  char * f_e_pos_x = "/tmp/fifo_est_pos_x";             // Motor x and inspection.
  char * f_e_pos_z = "/tmp/fifo_est_pos_z";             // Motor z and inspection.
  char * f_com_to_wd = "/tmp/fifo_command_to_wd";       // Command and watchdog.
  char * f_com_to_ins = "/tmp/fifo_command_to_ins";     // Command and inspection.

  // Creating all the fifos with create_fifo() function.

  create_fifo(f_com_to_mx);
  create_fifo(f_com_to_mz);
  create_fifo(f_e_pos_x);
  create_fifo(f_e_pos_z);
  create_fifo(f_com_to_ins);
  create_fifo(f_com_to_wd);

  // Calling the spawn() function created before to create the two motors
  // processes, motor x and motor z. We decided to pass two parameters as 
  // part of the argv[] array. They are the fifos between the two motors
  // processes and the command and the inspection processes.

  char * arg_list_1[] = { "./motor_x", f_com_to_mx ,f_e_pos_x, NULL };
  char * arg_list_2[] = { "./motor_z", f_com_to_mz,f_e_pos_z, NULL };

  pid_motor_x = spawn("./motor_x", arg_list_1); 
  pid_motor_z = spawn("./motor_z", arg_list_2);

  // Calling the sprintf() function to pass to the two arrays the PIDS of the
  // processes just called, this is helpful to pass them inside the argv[]
  // array of the process that we will call next, the watchdog.

  sprintf(pid_motor_x_a, "%d", pid_motor_x);
  sprintf(pid_motor_z_a, "%d", pid_motor_z);

  // Calling again spawn() to create the watchdog process. We want to pass
  // inside the function the command - watchdog fifo and the pids of the 
  // two processes just called (motor_x and motor_z).

  char * arg_list_5[] = {"./watchdog",f_com_to_wd, pid_motor_x_a, pid_motor_z_a, NULL };
  pid_wd = spawn("./watchdog", arg_list_5);

  // Passing the process ID of the watchdog to the relative array.

  sprintf(pid_wd_a, "%d", pid_wd);

  // Calling two last times spawn() function to create the command and inspection
  // processes. We want to pass different variables to each one:
  // - Command: we have the PID of the watchdog process, the fifos between command 
  //            and inspection, motor x, motor z and watchdog processes.
  // - Inspection: we have the PIDs of motor x and motor z. We also do have the 
  //               fifos between inspection and command, motor x and motor z.

  char * arg_list_3[] = { "/usr/bin/konsole",  "-e", "./command", 
                          pid_wd_a,
                          f_com_to_ins,f_com_to_mx, 
                          f_com_to_mz,f_com_to_wd,
                          (char*)NULL };
  pid_command = spawn("/usr/bin/konsole", arg_list_3);

  char * arg_list_4[] = { "/usr/bin/konsole",  "-e", "./inspection", 
                          pid_motor_x_a, 
                          pid_motor_z_a,
                          f_com_to_ins,
                          f_e_pos_x,
                          f_e_pos_z,
                          pid_wd_a,
                          (char*)NULL };
  pid_inspection = spawn("/usr/bin/konsole", arg_list_4);

  // Opening logfile in append mode.

  FILE *out2 = fopen("debug.txt", "a");
  fprintf(out, "PID ./master: %d\n", pid_master);
  fprintf(out, "PID ./motor_x: %d\n", pid_motor_x);
  fprintf(out, "PID ./motor_z: %d\n", pid_motor_z);
  fprintf(out, "PID ./watchdog: %d\n", pid_wd);
  CHECK(fclose(out2));

  // Now we make the master process waiting for one of the processes to die and
  // we close the debug file. Then, we unlink all the fifos to don't let them
  // inside the project folder. At the end we return 0.
  // The exit status of all the programs except the command process if everything 
  // is okay it should be 9 (SIGKILL exit status).
  pid_t pid_dead;
  pid_dead = CHECK(wait(&status));

  if(WIFEXITED(status)){

    printf("Exit status ./command: %d\n", WEXITSTATUS(status));

    int status_motor_x = CHECK(kill(pid_motor_x, SIGKILL));
    int status_motor_z = CHECK(kill(pid_motor_z, SIGKILL));
    int status_wd = CHECK(kill(pid_wd, SIGKILL));
    int status_ins = CHECK(kill(pid_inspection, SIGKILL));

    printf("/motor_x Killed with status: %d\n", status_motor_x);
    printf("./motor_z Killed with status: %d\n", status_motor_z);
    printf("./watchdog Killed with status: %d\n", status_wd);
    printf("./inspection Killed with status: %d\n\n", status_ins);

  }
  else if(WIFSIGNALED(status)){
    printf("Child with PID: %d dead Unexpectedly, SIGNAL NUMBER %d wrongly managed!!\n", pid_dead,  WTERMSIG(status));
    exit(-2);
  }
  else{
    printf("Child with PID: %d dead Unexpectedly with status: %d\n\n", pid_dead,status );
    exit(-1);
  }

  // UNLINKING ALL THE PIPES 
  CHECK(unlink(f_com_to_mx));
  CHECK(unlink(f_com_to_mz));
  CHECK(unlink(f_e_pos_x));
  CHECK(unlink(f_e_pos_z));
  CHECK(unlink(f_com_to_ins));
  CHECK(unlink(f_com_to_wd));
  
  return 0;

}