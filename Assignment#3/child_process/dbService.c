#include "Utilities.h"

int main()
{
    int fd, FifoBytesCounter = 0, Opcode = 0;
    sem_unlink(READER_SEMAPHORE);
    sem_unlink(WRITER_SEMAPHORE);
    sem_t* flightsDBSema = sem_open(READER_SEMAPHORE, O_CREAT, 0660, 0);
    sem_t* flightsServiceSema = sem_open(WRITER_SEMAPHORE, O_CREAT, 0660, 0);
    if (flightsDBSema == SEM_FAILED || flightsServiceSema == SEM_FAILED)
    {
        perror("Semaphore failed.\n Exiting program...\n");
        exit(-1);
    }
    const char* myfifo = "../tmp/filghts_fifo";

    unlink(myfifo);
    if (mkfifo(myfifo, 0666) == -1)
    {
          fprintf(stderr, "FIFO failed.\n");
    }

    if ((fd = open(myfifo, O_RDWR)) == -1)
    {
        printf("Error number %d\n", errno);
        perror("Error");
    }

    prepareToUnzip();
    char** dirList = NULL;
    int listSize = 0;
    dirList = createDirList(&listSize);
    DB* dataBase = getDataBase(listSize, dirList);
    
    for (int i = 0; i < listSize; i++)
    {
        free(dirList[i]);
    }
    
    if (listSize > 0)
        free(dirList);
    
    while (true)
    {
        sem_wait(flightsDBSema);
        FifoBytesCounter = read(fd, &Opcode, sizeof(Opcode));
        while (FifoBytesCounter != sizeof(Opcode))
        {
            FifoBytesCounter += read(fd, &Opcode, sizeof(Opcode));
        }

        child_process(fd , Opcode, &dataBase);

        if (Opcode == 6)
        {
            sem_post(flightsServiceSema);
            sem_close(flightsDBSema);
            sem_close(flightsServiceSema);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}