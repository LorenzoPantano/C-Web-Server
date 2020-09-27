#include "cache.h"



/**
 * Filename is of type: ../files/img/img.jpg
 * CACHE NAMING
 * {imgname}-{quality}-{sizeW}-{sizeH}
*/

int verifyCache(char *filename, int quality, int screenWidth, int screenHeight) {
    
    char *imgName = extractActualName(filename);
    imgName = strtok(imgName, "."); //Remove extension .jpg
    char *cacheName;
    sprintf(cacheName, "%s-%d-%d-%d.jpg", imgName, quality, screenWidth, screenHeight);

    printf("VERIFY CACHE: %s\n", cacheName);
    int res = searchInDir("../files/cache", 0, cacheName);
    return res;

}

/**
 * Build Cache Name
 * Returns the string with the cache name format for the file
 * "../files/cache/imgname-{quality}-{screenWidth}-{screenHeight}"
 * @param file: The actual filename {imgname} (../files/img/imgname.jpg)
 * @param quality: The integer quality {quality}
 * @param screenWidth: The integer screenWidth {screenWidth}
 * @param screenHeight: The integer screenHeight {screenHeight}
 * @return The cacheName string for the file
*/
char *buildCacheName(char *file, int quality, int screenWidth, int screenHeight) {
    printf("BUILD CACHE NAME\n");
    char *imgName = extractActualName(file);  //Extract imgname = imgname.jpg
    printf("BUILD CACHE NAME IMGNAME: %s\n", imgName);
    imgName = strtok(imgName, "."); //Remove .jpg extension
    char *cacheName;
    sprintf(cacheName, "%s-%d-%d-%d.jpg", imgName, quality, screenWidth, screenHeight);
    printf("BUILD CACHE NAME CACHENAME: %s\n", cacheName); //Cachename in  format imgname-q-sW-sH.jpg
    return cacheName;
}

int searchInDir(char *dir, int depth, char *cacheName)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
                continue;
            printf("%*s%s/\n",depth,"",entry->d_name);
            if (strcmp(entry->d_name, cacheName) == 0) {
                chdir("../../bin");
                return 0;
            }
        }
        else {
            if (strcmp(entry->d_name, cacheName) == 0) {
                chdir("../../bin");
                return 0;
            }
        }
    }
    chdir("../../bin");
    closedir(dp);
    return 1;
}