#include "img.h"

const char *subsampName[TJ_NUMSAMP] = {
  "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"
};

char *compressAndCacheImg(char *imgname, int isMobile, int screenWidth, int screenHeight, float quality) {
    
    //Extract img name with extenstion
    printf("ACTUAL NAME BEFORE COMPRESSING: %s\n", imgname);
    char *actualName = extractActualName(imgname);
    char *returnName;
    strcpy(returnName, actualName);
    returnName = strtok(returnName, ".");
    printf("ACTUAL NAME AFTER COMPRESSING: %s\n", actualName);
    

    printf("Actual Name: %s\n", actualName);
    printf("Is Mobile: %d\n", isMobile);
    printf("Screen Width: %d\n", screenWidth);
    printf("Screen Width: %d\n", screenHeight);
    
    //Check if is mobile
    if (isMobile) {
        //Compress and resize
        printf("Is Mobile, Compressing and resizing...\n");
        int res = compressAndResize(actualName, screenWidth, screenHeight, quality);
        if (res) {
            printf("Error during compression for mobile, sending highest quality\n");
            return NULL;
        } else {
            return returnName;
        }

    }
}

int compressAndResize(char *actualName, int screenWidth, int screenHeight, float quality) {

    /**
     * Calculate resized width and resized height.
     * This operation is based on the formats accepted by the turbojpeg-API:
     * scale M/N = Scale the input image by a factor of M/N when decompressing it.
     * (M/N = 2/1, 15/8, 7/4, 13/8, 3/2, 11/8, 5/4, 9/8, 1/1, 7/8, 3/4, 5/8, 1/2, 3/8, 1/4, or 1/8).
     * The closest ratio to the actual screen width and height values will be used.
     * The values > 1 are not used, because we won't enlarge the image in any case.
     * See roundClosestToFloat() in utils.c
    */


    int resizedWidth, resizedHeight;
    int *closest;
    closest = roundToClosestScalingFactor((float)screenWidth/(float)screenHeight);

    /* TURBOJPEG API Variables */
    tjscalingfactor scalingFactor = { closest[0], closest[1] };
    tjhandle tjInstance = NULL;
    int outSubsamp = -1, outQual = -1;
    int inSubsamp;
    FILE *jpegFile = NULL;
    unsigned char *imgBuf = NULL, *jpegBuf = NULL;
    long size;
    unsigned long jpegSize = 0;
    int pixelFormat = TJPF_UNKNOWN;
    int flags = 0;
    int inColorspace;
    int width, height;

    printf("Scaling Factors: %d %d\n", scalingFactor.num, scalingFactor.denom); //Okay till here

    tjscalingfactor *scalingFactors = NULL;
    int numScalingFactors = 0;
    if ((scalingFactors = tjGetScalingFactors(&numScalingFactors)) == NULL) {
        printf("Error during API call to TurboJPEG\n");
        return 1;
    }

    /* Read the JPEG file into memory. */
    char filename[512] = "";
    strcat(filename, "../files/img/");
    strcat(filename, actualName);

    if ((jpegFile = fopen(filename, "rb")) == NULL) {
        printf("JPEG: Error opening file\n");
        return 1;
    }
    if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) || fseek(jpegFile, 0, SEEK_SET) < 0) {
        printf("JEPG: Error determining input file size\n");
        return 1;
    }
    if (size == 0) {
        printf("JPEG: Input file contains no data\n");
        return 1;
    }
    jpegSize = (unsigned long)size;

    printf("Read file okay\n");
    
    /* Allocating JPEG buffer */
    if ((jpegBuf = (unsigned char *)tjAlloc(jpegSize)) == NULL) {
        printf("JPEG: Error allocating Jpeg buffer\n");
        return 1;
    }
    if (fread(jpegBuf, jpegSize, 1, jpegFile) < 1) {
        printf("JPEG: Error reading input file into memory\n");
        return 1;
    }

    printf("Allocating buffer okay\n");

    /* Close the open file once read into memory */
    fclose(jpegFile);  
    jpegFile = NULL;

    /* Instantiate turbo jpeg decompressor */
    if ((tjInstance = tjInitDecompress()) == NULL) {
        printf("JPEG: Error inizializing decompressor\n");
        return 1;
    }

    if (tjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height, &inSubsamp, &inColorspace) < 0) {
        printf("JPEG: Error reading Jpeg header\n");
        return 1;
    }
    
    /* Calculate resized width and height */
    resizedWidth = TJSCALED(width, scalingFactor);
    resizedHeight = TJSCALED(height, scalingFactor);
    if (outSubsamp < 0)
      outSubsamp = inSubsamp;
    pixelFormat = TJPF_BGRX;

    printf("Resized width and height okay\n");

    /* Allocating image buffer */
    if ((imgBuf = (unsigned char *)tjAlloc(resizedWidth * resizedHeight * tjPixelSize[pixelFormat])) == NULL) {
        printf("JPEG: Error allocating uncompressed image buffer\n");
        return 1;
    }
    /* Decompressing JPEG Image into the image buffer */
    if (tjDecompress2(tjInstance, jpegBuf, jpegSize, imgBuf, resizedWidth, 0, resizedHeight, pixelFormat, flags) < 0) {
        printf("JPEG: Error decompressing JPEG Image\n");
        return 1;
    }

    printf("Allocating img buffer and decompressing to img buffer okay\n");
    
    tjFree(jpegBuf);  
    jpegBuf = NULL;
    tjDestroy(tjInstance);  
    tjInstance = NULL;
    jpegBuf = NULL;  /* Dynamically allocate the JPEG buffer */

    /* Setting the quality of the compression (DEFAULT = 95) */
    if (outQual < 0) outQual = DEFAULT_QUALITY;
    //printf(", %s subsampling, quality = %d\n", subsampName[outSubsamp], outQual);
    printf("Quality okay : %d\n", outQual);

    /* Initializing the compressor */
    if ((tjInstance = tjInitCompress()) == NULL) {
        printf("JPEG: Error initializing compressor\n");
        return 1;
    }


    /* Compressing image */
    if (tjCompress2(tjInstance, imgBuf, resizedWidth, 0, resizedHeight, pixelFormat, &jpegBuf, &jpegSize, outSubsamp, outQual, flags) < 0) {
        printf("JPEG: Error compressing image\n");
        printf("%s\n", tjGetErrorStr2(tjInstance));
        return 1;
    }

    printf("Compressing okay\n");

    /* Deallocate resources for the compression */
    tjDestroy(tjInstance);  
    tjInstance = NULL;


    /* Write the JPEG image to disk. */

    //Building cache name
    char cacheName[512] = "";
    char cacheNamePath[512] = "../files/cache/";
    actualName = strtok(actualName, ".");
    printf("ACTUAL NAME COMPRESSION: %s\n", actualName);
    sprintf(cacheName, "%s-%.2f-%d-%d.jpg", actualName, quality, screenWidth, screenHeight);
    strcat(cacheNamePath, cacheName);

    //Write to disk
    if ((jpegFile = fopen(cacheNamePath, "wb")) == NULL) {
        printf("JPEG: Error opening output file\n");
        return 1;
    }
    if (fwrite(jpegBuf, jpegSize, 1, jpegFile) < 1) {
        printf("JPEG: Error writing output file\n");
        return 1;
    }

    printf("Writing to disk okay\n");

    /* Deallocate resources */
    tjDestroy(tjInstance);  tjInstance = NULL;
    fclose(jpegFile);  jpegFile = NULL;
    tjFree(jpegBuf);  jpegBuf = NULL;
    tjFree(imgBuf);
    if (tjInstance) tjDestroy(tjInstance);
    tjFree(jpegBuf);
    if (jpegFile) fclose(jpegFile);
    
    return 0;
}