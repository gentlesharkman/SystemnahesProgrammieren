//
// Created by domi on 30.10.25.
//
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // fork a process
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        pid = getpid(); /*Holt ID des Prozesses selber*/
        pid_t ppid = getppid(); /*Holt ID des Parent prozesses*/
        printf("This is child process (pid=%d) with parent %d.\n", pid, ppid);
        // child sleeps for 5 secods
        sleep(5);
        exit(0);
    } else if (pid > 0) {
        // parent process
        pid = getpid();
        printf("This is the parent process with process id %d.\n", pid);
        // wait for child to finish
        pid = wait(NULL);   /*Wait wartet bis die Child jobs fertig sind*/
        //printf("%d", pid);
        if (pid == -1) {
            printf("Wait failed.\n");
            exit(1);
        }
        printf("Child %d finished.\n", pid);
        exit(0);
    } else {
        printf("Fork failed.\n");
        exit(1);
    }
}