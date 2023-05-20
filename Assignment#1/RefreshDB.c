#include "Utilities.h"

//this function calls script to refrsh air ports data base 
void main()
{
    int sizeOfDirList = 0;
    char** dirList = createDirList(&sizeOfDirList);
    loadDatabase(sizeOfDirList,dirList);
    for (int i = 0; i < sizeOfDirList; i++)
    {
        free(dirList[i]);
    }
    free(dirList);
}