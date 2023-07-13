#include "Utilities.h"

//Checks whether a flightsDB folder exists.
bool doesDBFolderExists()
{
    bool output = false;
    DIR *dir;
    struct dirent *entry;

    dir = opendir("..");

    while (dir != NULL && (entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, "flightsDB") == 0) {
            output = true;
            break;
        }
    }

    closedir(dir);
    return output;
}

//Creates an array of current airpots in the flightsDB directory.
char** createDirList(int* size)
{
    char** output = NULL;
    int phySize = 1, logSize = 0;

    output = (char**)malloc(sizeof(char*) * phySize);
    checkAllocation(output);

    DIR* directory = opendir("../flightsDB/");
    checkAllocation(directory);

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL)
    {
        // Exclude hidden files/directories that start with a dot
        if (entry->d_name[0] != '.' && strlen(entry->d_name) == NAME_LEN)
        {
            if (logSize == phySize)
            {
                phySize *= 2;
                output = (char**)realloc(output, phySize * sizeof(char*));
                checkAllocation(output);
            }

            output[logSize] = (char*)malloc(sizeof(char) * (NAME_LEN + 1));
            checkAllocation(output[logSize]);
            strcpy(output[logSize], entry->d_name);
            output[logSize][NAME_LEN] = '\0';

            logSize++;
        }
    }

    if (logSize > 0)
    {
        output = (char**)realloc(output, logSize * sizeof(char*));
        checkAllocation(output);
    }

    *size = logSize;
    closedir(directory);
    return output;
}

//Removes a directory.
void removeDirectory(const char* path)
{
    DIR* directory = opendir(path);
    struct dirent* item;

    if (directory != NULL)
    {
        while ((item = readdir(directory)) != NULL)
        {
            if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0)
            {
                char itemPath[1024];
                snprintf(itemPath, sizeof(itemPath), "%s/%s", path, item->d_name);
                struct stat statbuf;
                if (stat(itemPath, &statbuf) == 0)
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        removeDirectory(itemPath);
                    }
                    
                    else
                    {
                        remove(itemPath);
                    }
                }
            }
        }
        closedir(directory);
    }

    rmdir(path);
}