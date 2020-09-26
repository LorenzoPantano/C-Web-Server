#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include "httpFieldsStruct.h"

int isANumber(const char *string);
int readn(int fd, void *buf, size_t n);             //Read & Write functions to read/write up to n bytes;
ssize_t writen(int fd, const void *buf, size_t n);
int requestType(char *method);                      
int sendHeaderMessage(int socket, char *head, char *media, int file_size);
char* getContentType(char *path);
httpFieldsPtr parseHttpRequest(char *httpRequest);
char *extractSubString(char *httpRequest, char *pattern1, char *pattern2);
char *extracImageQualityAccept(char *acceptHeader);
int *roundToClosestScalingFactor(float num);
char *extractActualName(char *imgname);