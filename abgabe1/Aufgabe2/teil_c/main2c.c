//
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    int nmbPrzs = atoi(argv[1]); /*in argv[1] kommandozeilenargumente gespeichert*/
    pid_t pid;
    for (int i = 0; i < nmbPrzs; i++) {
        pid = fork();
        if (pid == 0) {
            printf("This is child process %d \n", i + 1);
            sleep(1);
            exit(0);
        }
        if (pid > 0) {
            wait(NULL);
        }
    }
    printf("Parent process finished");
    return 0;
}