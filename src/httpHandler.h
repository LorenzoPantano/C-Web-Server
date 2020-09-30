// Libraries
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/sendfile.h>

//Header files
#include "errorHandler.h"
#include "51DegreesDeviceDetection/deviceDetection.h"
#include "img.h"

//Variables
int bytes_sent;
int fd;

//Function Prototypes
int handleGETRequest(int socket, char *filepath, char *userAgent, float quality, int cacheDim);
int handleHEADRequest(int socket, char *filepath);