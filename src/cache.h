#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"

int verifyCache(char *filename, int quality, int screenWidth, int screenHeight);
char *buildCacheName(char *file, int quality, int screenWidth, int screenHeight);
int searchInDir(char *dir, int depth, char *cacheName);