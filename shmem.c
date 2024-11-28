#include <stdio.h>      // For printf and standard input/output
#include <unistd.h>     // For fork and process control functions
#include <sys/types.h>  // For defining data types like pid_t
#include <sys/ipc.h>    // For shared memory and IPC (Inter-Process Communication) functions
#include <sys/shm.h>    // For shared memory functions like shmget, shmat, shmdt

#define SHMSIZE 128     // Define the size of the shared memory block (128 bytes)
#define SHM_R 0400      // Read permission for shared memory
#define SHM_W 0200      // Write permission for shared memory

int main(int argc, char **argv) {
    // Define a structure to hold shared data
    struct shm_struct {
        int buffer;      // Data buffer to store an integer
        unsigned empty;  // Flag to indicate if the buffer is empty (1 = full, 0 = empty)
    };

    // Declare a pointer to the shared memory structure
    volatile struct shm_struct *shmp = NULL;

    // Pointer to shared memory address
    char *addr = NULL;

    // Process ID variable
    pid_t pid = -1;

    // Variables for producer (var1) and consumer (var2)
    int var1 = 0, var2 = 0;

    // Shared memory ID
    int shmid = -1;

    // Structure to hold shared memory metadata
    struct shmid_ds *shm_buf;

    // Allocate a chunk of shared memory
    shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
    // - IPC_PRIVATE: Creates a unique shared memory key
    // - SHMSIZE: Size of the shared memory block (128 bytes)
    // - IPC_CREAT | SHM_R | SHM_W: Create shared memory with read/write permissions

    // Attach the shared memory to the process
    shmp = (struct shm_struct *)shmat(shmid, addr, 0);
    // - shmid: Shared memory ID
    // - addr: Let the system decide where to attach memory
    // - 0: Default flags for read/write access

    // Initialize the `empty` flag to 0 (buffer is empty initially)
    shmp->empty = 0;

    // Create a child process using fork()
    pid = fork();

    if (pid != 0) {  // Parent process (Producer)
        while (var1 < 5) { // Produce data until var1 reaches 100
            var1++; // Increment the producer's data
            while (shmp->empty == 1); // Busy wait until the buffer is empty
            printf("Sending %d\n", var1); // Print the data being sent
            fflush(stdout); // Ensure the output is printed immediately
            shmp->buffer = var1; // Write the data to the shared memory buffer
            shmp->empty = 1; // Mark the buffer as full
        }
        // Detach the shared memory from the parent process
        shmdt(addr);

        // Remove the shared memory segment after use
        shmctl(shmid, IPC_RMID, shm_buf);
    } else { // Child process (Consumer)
        while (var2 < 5) { // Consume data until var2 reaches 100
            while (shmp->empty == 0); // Busy wait until the buffer is full
            var2 = shmp->buffer; // Read the data from the shared memory buffer
            shmp->empty = 0; // Mark the buffer as empty
            printf("Received %d\n", var2); // Print the data being received
            fflush(stdout); // Ensure the output is printed immediately
        }
        // Detach the shared memory from the child process
        shmdt(addr);

        // Remove the shared memory segment (redundant, as parent removes it)
        shmctl(shmid, IPC_RMID, shm_buf);
    }

    return 0; // Exit the program
}
