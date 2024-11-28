#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    // Variables for child process IDs
    pid_t pid1, pid2;
    unsigned i;
    unsigned niterations = 100;

    // First fork
    pid1 = fork();
    if (pid1 == 0) {
        // First child process
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
        printf("\nChild 1 (A) => Own PID: %d, Parent PID: %d\n", getpid(), getppid());
        return 0;
    }

    // Second fork
    pid2 = fork();
    if (pid2 == 0) {
        // Second child process
        for (i = 0; i < niterations; ++i)
            printf("C = %d, ", i);
        printf("\nChild 2 (C) => Own PID: %d, Parent PID: %d\n", getpid(), getppid());
        return 0;
    }

    // Parent process
    for (i = 0; i < niterations; ++i)
        printf("B = %d, ", i);
    printf("\nParent => Own PID: %d\n", getpid());
    printf("Parent => Child 1 PID: %d, Child 2 PID: %d\n", pid1, pid2);

    return 0;
}
