#ifndef UTILITIES_H
#define UTILITIES_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <zip.h>
#include <sys/stat.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/sem.h>

#define MAX_SIZE 300
#define READ 0
#define WRITE 1
#define SUCCESS 0
#define FINISH 1
#define READER_SEMAPHORE "/readersema"
#define WRITER_SEMAPHORE "/writersema"

void printMenu();
char** readInput(int* size, int choice);
bool isValidChar(char input);
void ReadOrWriteToPipe(char** output, int O_size, int fd, bool SIG);
void checkAllocation(void* pointer);

#endif
