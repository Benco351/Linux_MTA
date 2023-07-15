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

#define MAX_SIZE 128
#define READ 0
#define WRITE 1
#define FINISH 1
#define EMPTY_INPUT -1
#define SCRIPT_FAILURE -2
#define FETCH_DATA 1
#define INCOMING_FLIGHTS 2
#define FULL_SCHEDULE 3
#define AIRCRAFT_SCHEDULE 4
#define ZIP_DB 5
#define SHUTDOWN 6

#define DB_SERVICE_SEMAPHORE "/dbservicesemaphore"
#define FLIGHTS_SERVICE_SEMAPHORE "/flightservicesemaphore"

void printMenu();
char** readInput(int* size, int choice);
bool isValidChar(char input);
void ReadOrWriteToPipe(char** output, int O_size, int fd, bool SIG);
void checkAllocation(void* pointer);
#endif