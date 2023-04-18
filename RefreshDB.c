#include "Utilities.h"

void main()
{
    int sizeOfDirList = 0;
    char** dirList = createDirList(&sizeOfDirList);
    loadDatabase(sizeOfDirList,dirList);
}