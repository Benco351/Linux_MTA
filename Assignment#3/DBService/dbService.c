#include "Utilities.h"

int main()
{
    int FIFO, FifoBytesCounter = 0, Opcode = 0;
    struct stat stat_p;
    // sem_unlink(DB_SERVICE_SEMAPHORE);
    // sem_unlink(FLIGHTS_SERVICE_SEMAPHORE);
    sem_t* DBServiceSema = sem_open(DB_SERVICE_SEMAPHORE, O_CREAT , 0660, 0);
    sem_t* flightsServiceSema = sem_open(FLIGHTS_SERVICE_SEMAPHORE, O_CREAT , 0660, 0);
    if (DBServiceSema == SEM_FAILED || flightsServiceSema == SEM_FAILED)
    {
        perror("Semaphore failed.\n Error");
        exit(-1);
    }
    const char* myfifo = "/shared-volume/filghts_fifo";
    stat(myfifo,&stat_p);
    if(!file_exists(myfifo) || S_ISFIFO(stat_p.st_mode) == 0)
    {
        if (mkfifo(myfifo, 0666) == -1)
        {
            fprintf(stderr, "FIFO failed.\n");
        }
    }
    // unlink(myfifo);

    if ((FIFO = open(myfifo, O_RDWR)) == -1)
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
    
    printf("DB Service is up and waiting for requests.\n");
    
    while (true)
    {
        sem_wait(DBServiceSema);
        FifoBytesCounter = read(FIFO, &Opcode, sizeof(Opcode));
        while (FifoBytesCounter != sizeof(Opcode))
        {
            FifoBytesCounter += read(FIFO, &Opcode, sizeof(Opcode));
        }

        dbServiceProcess(FIFO, Opcode, &dataBase);

        if (Opcode == 6)
        {
            sem_close(DBServiceSema);
            sem_close(flightsServiceSema);
            remove(myfifo);
            exit(EXIT_SUCCESS);
        }

        sem_post(flightsServiceSema);
    }

    return 0;
}