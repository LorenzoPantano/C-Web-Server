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

    //File operations
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
    }
    else {
        //printf("Bytes sent from header message: %d\n", bytes_sent);  //debug
        bytes_sent = sendfile(socket, fd, NULL, file_size);
        //printf("Bytes sent: %d\n", bytes_sent);  //debug
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
    
    

}

void handleHEADRequest(){

}