#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <turbojpeg.h>
#include "utils.h"
#include <semaphore.h>

sem_t mutex;

#define DEFAULT_QUALITY  95

char *compressAndCacheImg(char *imgname, int isMobile, int screenWidth, int screenHeight, float quality);
int compressAndResize(char *actualName, int screenWidth, int screenHeight, float quality);
int compress(char *actualName);