#include "Utillities.h"

int main ()
{
    int fd, FifoBytesCounter = 0,Opcode = 0;
    const char* myfifo ="./tmp/filghts_fifo";


    if(mkfifo(myfifo,(mode_t)666) == -1)
    {
          fprintf(stderr, "FIFO failed.\n");
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
        if(fd = open(myfifo,O_RDWR) == -1)
        {
            printf("Error number %d\n", errno);
            perror("Program");
        }
        
        FifoBytesCounter = read(fd, &Opcode, sizeof(Opcode));
        while (FifoBytesCounter != sizeof(Opcode))
        {
            FifoBytesCounter += read(fd, &Opcode, sizeof(Opcode));
        }

        child_process(fd , Opcode, &dataBase);
        close(fd);
    }



    return 0;
}