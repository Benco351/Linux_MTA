#include "Utilities.h"

//Checks whether flightsDB.zip exists.
bool doesZipExists()
{
    bool output = false;
    DIR *dir;
    struct dirent *entry;

    dir = opendir("..");

    while (dir != NULL && (entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, "flightsDB.zip") == 0) {
            output = true;
            break;
        }
    }

    closedir(dir);
    return output;
}

//Starts the unzipping of the zip file.
void prepareToUnzip()
{
    char cwd[MAX_SIZE], tmp[MAX_SIZE];
    getcwd(cwd, sizeof(cwd));
    int size = strlen(cwd);
    cwd[size - 6] = '\0';
    strcpy(tmp, cwd);
    strcat(tmp, "/flightsDB.zip");
    strcat(cwd, "/flightsDB");
    bool DBFolderExists = doesDBFolderExists();

    if (!DBFolderExists)
    {
        mkdir(cwd,0777);
    }

    bool zipExists = doesZipExists();
    if (zipExists)
    {  
        if (cwd != NULL)
        {
            (void)unzipFolder(tmp, cwd);
        }
    }
}

//Starts the zipping of the DB folder.
void prepareToZip()
{
    char tmp[MAX_SIZE], cwd[MAX_SIZE];
    int size = 0;
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        int size = strlen(cwd);
        cwd[size - 6] = '\0';
        strcpy(tmp,cwd);
        strcat(tmp,"/flightsDB.zip");
        strcat(cwd,"/flightsDB");
        bool DBFolderExists = doesDBFolderExists();

        if (DBFolderExists)
        {
            (void)zipFolder(cwd, tmp);
        }  
    }
}

//Adds a file into the zip.
void addFileToZip(struct zip* archive, const char* filePath, const char* entryName)
{
    struct zip_source* source = zip_source_file_create(filePath, 0, -1, 0);
    if (!source) 
    {
        return;
    } 

    int result = zip_add(archive, entryName, source);
    if (result < 0) 
    {
      zip_source_free(source);
    }

}

//Adds a folder into the zip.
void addFolderToZip(struct zip* archive, const char* folderPath, const char* baseDir)
{
    DIR* dir;
    struct dirent* entry;
    char filePath[MAX_SIZE];

    dir = opendir(folderPath);
    if (!dir) 
    {
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(filePath, sizeof(filePath), "%s/%s", folderPath, entry->d_name);

        struct stat fileStat;
        if (stat(filePath, &fileStat) == 0)
        {
            if (S_ISREG(fileStat.st_mode))
            {
                char entryName[MAX_SIZE];
                snprintf(entryName, sizeof(entryName), "%s%s%s", baseDir, (baseDir[strlen(baseDir) - 1] == '/') ? "" : "/", entry->d_name);
                addFileToZip(archive, filePath, entryName);
            }
            
            else if (S_ISDIR(fileStat.st_mode))
            {
                char subFolderBaseDir[MAX_SIZE];
                snprintf(subFolderBaseDir, sizeof(subFolderBaseDir), "%s%s%s", baseDir, (baseDir[strlen(baseDir) - 1] == '/') ? "" : "/", entry->d_name);
                addFolderToZip(archive, filePath, subFolderBaseDir);
            }
        }
    }

    closedir(dir);
}

//Starts the zipping of a folder.
int zipFolder(const char* folderPath, const char* zipFilePath) 
{
    struct zip* archive = zip_open(zipFilePath, ZIP_CREATE | ZIP_TRUNCATE, NULL);
    if (!archive) 
    {
        return 1;
    }

    addFolderToZip(archive, folderPath, "");
    if (zip_close(archive) < 0) 
    {
        return 1;
    }

    removeDirectory(folderPath);
    return 0;
}

//Unzips to a folder.
int unzipFolder(const char* zipFilePath, const char* destinationFolder)
{
    struct zip* archive = zip_open(zipFilePath, 0, NULL);
    if (!archive)
    {
        return 1;
    }

    mkdir(destinationFolder, S_IRWXU | S_IRWXG | S_IRWXO);

    int numEntries = zip_get_num_entries(archive, 0);
    for (int i = 0; i < numEntries; i++)
    {
        struct zip_stat entryStat;
        if (zip_stat_index(archive, i, 0, &entryStat) == 0)
        {
            if (entryStat.name[strlen(entryStat.name) - 1] == '/')
            {
                // Skip directories
                continue;
            }

            char entryName[MAX_SIZE];
            snprintf(entryName, sizeof(entryName), "%s%s%s", destinationFolder, (destinationFolder[strlen(destinationFolder) - 1] == '/') ? "" : "/", entryStat.name);

            struct zip_file* file = zip_fopen_index(archive, i, 0);
            if (!file)
            {
                continue;
            }

            // Create directory if it doesn't exist
            char dirName[MAX_SIZE];
            strncpy(dirName, entryName, sizeof(dirName));
            char* lastSlash = strrchr(dirName, '/');
            if (lastSlash)
            {
                *lastSlash = '\0';
                mkdir(dirName, 0777);
            }

            FILE* outputFile = fopen(entryName, "wb");
            if (!outputFile)
            {
                zip_fclose(file);
                continue;
            }

            char buffer[4096];
            zip_int64_t bytesRead;
            while ((bytesRead = zip_fread(file, buffer, sizeof(buffer))) > 0)
            {
                fwrite(buffer, 1, bytesRead, outputFile);
            }

            fclose(outputFile);
            zip_fclose(file);

            // Set file permissions
            chmod(entryName,(mode_t)0644);
        }
    }

    zip_close(archive);
    remove(zipFilePath);

    return 0;
}