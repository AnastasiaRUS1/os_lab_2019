
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s seed arraysize\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }
    
    if (pid == 0) {
        // Дочерний процесс
        execlp("./sequential_min_max", "./sequential_min_max", argv[1], argv[2], NULL);
        perror("execlp failed");
        exit(1);
    } else {
        // Родительский процесс
        wait(NULL);
    }

    return 0;
}