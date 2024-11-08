
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int main() {
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            
            printf("Child process %d created\n", i + 1);
            exit(0);
        } else if (pid > 0) {
            
            printf("Parent process waiting before it continues...\n");
            
            sleep(1);
        } else {
            
            perror("fork failed");
            exit(1);
        }
    }

    
    while (1) {
        printf("Parent process waiting infinitely...\n");
        sleep(2);
    }

    return 0;
}
