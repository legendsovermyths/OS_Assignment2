#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define PATH_LEN 256

typedef struct
{
    int request_flag;
    char data[SHM_SIZE - sizeof(int)]; // Space for data
} Request;

enum REQUEST_TYPE
{
    Register,
    Airthmatic,
    EvenOrOdd,
    IsPrime,
    IsNegative
};

key_t generateKey(const char *str)
{
    struct stat fileStat;

    if (stat(str, &fileStat) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    return ftok(str, 'R');
}

int createSharedMemory(key_t key, size_t size)
{
    int shmid = shmget(key, size, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

char *CREATE_CLIENT_FILE(const char *CLIENT_NAME)
{
    FILE *file;
    char *CLIENTS = "CLIENTS";
    char *absolute_file_path = (char *)malloc(PATH_LEN * sizeof(char));
    char current_directory[PATH_LEN];

    if (getcwd(current_directory, sizeof(current_directory)) == NULL)
    {
        perror("Error getting current working directory");
        exit(EXIT_FAILURE);
    }

    snprintf(absolute_file_path, 1024, "%s/%s/%s", current_directory, CLIENTS, CLIENT_NAME);
    file = fopen(absolute_file_path, "w");

    if (file == NULL)
    {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }

    printf("File created: %s\n", absolute_file_path);
    fclose(file);
    return absolute_file_path;
}

char *CREATE_CLIENT_COMM_CHANNEL(const char *CLIENT_NAME)
{
    const char *sharedMemoryString = CREATE_CLIENT_FILE(CLIENT_NAME);
    key_t key = generateKey(sharedMemoryString);
    int shmid = createSharedMemory(key, SHM_SIZE);

    printf("Shared memory created with key: %d\n", key);

    char *shmaddr = (char *)shmat(shmid, NULL, 0);

    if (shmaddr == (char *)(-1))
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return shmaddr;
}

char START_SERVER()
{
    return 'a';
}

int main()
{
    char *shmaddr = CREATE_CLIENT_COMM_CHANNEL("HELLO");

    // Create shared memory

    // Attach the shared memory segment

    // Writing data to shared memory
    strcpy(shmaddr, "Hello, shared memory!");

    // Reading data from shared memory
    printf("Data read from shared memory: %s\n", shmaddr);
    // Detach the shared memory segment
    if (shmdt(shmaddr) == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    // // Optionally, you can remove the shared memory segment
    // if (shmctl(shmid, IPC_RMID, NULL) == -1)
    // {
    //     perror("shmctl");
    //     exit(EXIT_FAILURE);
    // }

    return 0;
}
