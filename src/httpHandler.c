/*  This file contains the functions 
*   in which the http request is processed,
*   depending on the case if is a GET or HEAD request
*/

#include "httpHandler.h"



/**
 * Handles the GET request
 * @param socket the socket where to respond
 * @param filepath request file path
 * @return bytes actually sent if succesful, 0 or < 0 otherwise
*/
int handleGETRequest(int socket, char *filepath, char *userAgent, float quality){

    /**
     * TODO: Set up 51degress (n casin) or WURLF
     * - Pass the user agent string to 51degrees
     * - Retrieve device info
     * - If image is in cache for device return image from cache, otherwise
     * - Refractor image quality (only image) using ImageMagick
     * - Save image refractored in cache
     * - Return refractored image
    */
    
    char file[512] = "../files";   //Base directory

    //If nothing specified --> index.html
    if (strcmp(filepath,"/") == 0) {
        strcat(file, "/index.html");
    }
    else strcat(file, filepath);

    printf("Handling GET Request on socket %d, file: %s\nUser Agent: %s\n", socket, file, userAgent);

    uaInfoStructPtr detectionResult = detectDeviceFromUserAgent(userAgent);
    if (detectionResult == NULL){
        printf("Error\n");
    } else {
        printf("Success\n");
        printf("DETECTION RESULTS : \n");
        printf("isMobile: %d\n", detectionResult->isMobile);
        printf("Screen Width: %d\n", detectionResult->screenWidth);
        printf("Screen Height: %d\n", detectionResult->screenHeight);
    }

    /**
     * Skip index.html and favicon.ico files from optimization.
    */
    if ((strcmp(file, "../files/index.html")) == 0 || (strcmp(file, "../files/favicon.ico") == 0)) {
        //Send file directly
    } else {

        /**
         * Mobile Optimization:
         * Images for mobile devices are not only compressed
         * but also resized based on their screen sizes.
         * Different screen sizes and quality will produce different
         * compressed images to be put in cache.
        */

        if (detectionResult->isMobile == 1) {

            /**
             * Verify if file is in cache.
             * Mobile cache and not mobile are different.
             * Mobile optimized images are also resized for different 
             * screen sizes.
            */

            /**
             * Copy the file name string to another variable
             * to avoid messing up the "file" string variable.
             * This will be used often in the following lines.
            */
            char filename[512] = "";
            strcat(filename, file);
            
            int cacheResult = verifyCache(filename, (int)(quality*100), detectionResult->screenWidth, detectionResult->screenHeight );
            if (cacheResult == 0) {  //Cache hit

                strcpy(filename, "");
                strcat(filename, file);

                strcpy(file, "");
                strcat(file, "../files/cache/");
                char *cacheName = buildCacheName(filename,(int)(quality*100), detectionResult->screenWidth, detectionResult->screenHeight);
                strcat(file, cacheName);
                printf("FILE TO BE SENT ALREADY IN CACHE: %s\n", file);
                goto send;
            }

            /**
             * File not in cache.
             * Image is compressed and then sent.
             * In case of an error, the server will send the base image (not compressed)
            */

            char *actualName = compressAndCacheImg(file, 1, detectionResult->screenWidth, detectionResult->screenHeight, quality);
            if (actualName == NULL) {
                printf("Error compressing image: sending base quality\n");
            } else {
                strcpy(file, "");
                strcat(file, "../files/cache/");
                char cacheName[512] = "";
                sprintf(cacheName, "%s-%d-%d-%d.jpg", actualName, (int)(quality*100), detectionResult->screenWidth, detectionResult->screenHeight);
                strcat(file, cacheName);
                printf("FILE TO BE SENT: %s\n", file);
            }

        } else {
            
            /**
             * Not Mobile optimization
             * Images for desktop (or unknow devices)
             * are simply compressed using the quality factor.
            */

            /* Verify if file is already in cache */
           
            char filename[512] = "";
            strcat(filename, file);

            int cacheResult = verifyCache(filename, (int)(quality*100), 0, 0);
            if (cacheResult == 0) { //Cache hit
                char filename[512] = "";
                strcat(filename, file);
                strcpy(file, "");
                strcat(file, "../files/cache/");
                char *cacheName = buildCacheName(filename,(int)(quality*100), 0, 0);
                strcat(file, cacheName);
                printf("FILE TO BE SENT ALREADY IN CACHE (NOT MOBILE): %s\n", file);
            } else { //Cache miss
                printf("File not in cache, compressing...(NOT MOBILE)\n");
                char filename[512] = "";
                strcat(filename, file);
                char *actualName = compressAndCacheImg(filename, 0, 0, 0, quality);
                if (actualName == NULL) {
                    printf("Error compressing image: sending base quality\n");
                } else {
                    char filename[512] = "";
                    strcat(filename, file);
                    strcpy(file, "");
                    strcat(file, "../files/cache/");
                    char *cacheName = buildCacheName(filename, (int)(quality*100), 0, 0);
                    strcat(file, cacheName);
                    printf("FILE TO BE SENT: %s\n", file);
                }
            }

        }

    }

send:

    /* File operations */
    //Open
    fd = open(file, O_RDONLY);
    if (fd == -1) {
        if (errno == EACCES) {
            perror("Permission Denied\n");
            //Permission denied error 403 Forbidden;
            sendErrorMessage(socket, 403);                    
            pthread_exit((void *)EXIT_FAILURE);
        } else {
            perror("File does not exist HEAD\n");
            //File not found error 404
            sendErrorMessage(socket, 404);
            pthread_exit((void *)EXIT_FAILURE);
        }
    }
    //Metadata of the file to be sent in the header
    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;
    printf("File size: %d\n", file_size);
    char *mediaType = getContentType(file);   
    printf("Media type: %s\n", mediaType);


    /**
     * Send header message before sending the actual data,
     * so the client knows how to read the incoming bytes
     * as the HTTP Protocol.
     * */    
    bytes_sent = sendHeaderMessage(socket, "HTTP/1.1 200 OK", mediaType, file_size);
    if (bytes_sent <= 0) {
        //Internal server error 500
        sendErrorMessage(socket, 500);
        pthread_exit((void *)EXIT_FAILURE);
        return -1;
    }
    else {

        bytes_sent = sendfile(socket, fd, NULL, file_size);
        /**
         * If bytes sent is less then actual file size, send again
         * till the file is entirely sent.
         * */                    
        while (bytes_sent < file_size)                                     
        {                                                               
            bytes_sent = sendfile(socket, fd, NULL, file_size);         
            printf("Bytes sent: %d\n", bytes_sent);  //debug
            if (bytes_sent <= 0){
                perror("Error sending file");
                close(fd);
                sendErrorMessage(socket, 500);
                return -1;
            }
        }

        close(fd);  //Close file descriptor
        return bytes_sent;
    }

    return bytes_sent;
}


/**
 * Handles HEAD request
 * @param socket The returning socker
 * @param filepath The requested filepath
 * @return bytes actually sent, ( > 0 success , otherwise error)
*/
int handleHEADRequest(int socket, char *filepath){

    char file[512] = "../files";   //Base directory

    //If nothing specified --> index.html
    if (strcmp(filepath,"/") == 0) {
        strcat(file, "/index.html");
    }
    else strcat(file, filepath);

    printf("Handling HEAD Request on socket %d, file: %s\n", socket, file);


    /** File operations 
     * The file needs only to be opened to check his size
     * File is not sent in the HEAD Request
    */
    //Open
    fd = open(file, O_RDONLY);
    if (fd == -1) {
        if (errno == EACCES) {
            perror("Permission Denied\n");
            //Permission denied error 403 Forbidden;
            sendErrorMessage(socket, 403);                    
            pthread_exit((void *)EXIT_FAILURE);
        } else {
            perror("File does not exist HEAD\n");
            //File not found error 404
            sendErrorMessage(socket, 404);
            pthread_exit((void *)EXIT_FAILURE);
        }
    }
    //Metadata of the file to be sent in the header
    struct stat st;
    fstat(fd, &st);
    int file_size = st.st_size;
    printf("File size: %d\n", file_size);
    char *mediaType = getContentType(file);   
    printf("Media type: %s\n", mediaType);


    //Send header message  
    bytes_sent = sendHeaderMessage(socket, "HTTP/1.1 200 OK", mediaType, file_size);
    if (bytes_sent <= 0) {
        //Internal server error 500
        sendErrorMessage(socket, 500);
        pthread_exit((void *)EXIT_FAILURE);
        return -1;
    } else {
        return bytes_sent;
    }


}