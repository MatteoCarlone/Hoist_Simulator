#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

pid_t pid_command, pid_motor_x, pid_motor_z, pid_inspection, pid_wd;

int spawn(const char * program, char ** arg_list) {
  pid_t child_pid = fork();
  if (child_pid != 0)
    return child_pid;

  else {
    execvp (program, arg_list);
    perror("exec failed");
    return 1;
  }
}

void create_fifo (const char * name){
    if(mkfifo(name, 0666)==-1){
        if (errno != EEXIST){
        perror("Error creating named fifo\n");
        exit (1);
        }
    }
}

int main() {
char * arg_list_1[] = { "./motor_x", NULL, NULL };
char * arg_list_2[] = { "./motor_z", NULL, NULL };
char * arg_list_3[] = { "/usr/bin/konsole",  "-e", "./command", (char*)NULL };
char * arg_list_4[] = { "/usr/bin/konsole",  "-e", "./inspection", (char*)NULL };
char * arg_list_5[] = {"./wd", NULL, NULL };


create_fifo("fifo_command_to_mot_x");
create_fifo("fifo_command_to_mot_z");
create_fifo("fifo_est_pos_x");
create_fifo("fifo_est_pos_z");

pid_motor_x = spawn("./motor_x", arg_list_1);
pid_motor_z = spawn("./motor_z", arg_list_2);
pid_command = spawn("/usr/bin/konsole", arg_list_3);
pid_inspection = spawn("/usr/bin/konsole", arg_list_4);
//pid_wd = spawn("./wd", arg_list_5);

wait(NULL);
return 0;

}