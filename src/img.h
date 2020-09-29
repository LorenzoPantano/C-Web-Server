#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <turbojpeg.h>
#include "cache.h"

#define DEFAULT_QUALITY  100

char *compressAndCacheImg(char *imgname, int isMobile, int screenWidth, int screenHeight, float quality);
int compressAndResize(char *actualName, int screenWidth, int screenHeight, int quality);
int compress(char *actualName, int quality);