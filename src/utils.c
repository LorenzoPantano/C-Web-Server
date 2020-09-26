#include "utils.h"

/**
 * Check if the given string is made of digits
 * @param string the string to check
 * @return 1 if is made of digit, 0 otherwise
 */
int isANumber(const char *string)
{
    int lenght = strlen(string) - 1;
    for (int c = 0; c <= lenght; c++)
    {
        if (!isdigit(string[c]))
        {
            return 0;
        }
    }
    return 1;
}

/**
*   Return the equivalent integer for the 
*   method specified in the http request.
*   @param method the string method.
*   @return GET = 1 / HEAD = 2 / POST = 3
*/
int requestType(char *method) {
    int type = -1;
    if (strncmp(method, "GET\0", 4) == 0)
        type = 1;
    else if (strncmp(method, "POST\0", 5) == 0)
        type = 2;
    else if (strncmp(method, "HEAD\0", 5) == 0)
        type = 3;
    else
        type = -1;
    return type;
}


/**
 * Sends header of HTTP response message with HTTP 1.1 syntax
 * @param socket descriptor of the socket
 * @param head the first line of HTTP response message
 * @param media the myme-type of file
 * @param file_size dimension of file
 * @return number of bytes sent
 */
int sendHeaderMessage(int socket, char *head, char *media, int file_size)
{
    char keep_alive[] 	  = "\r\nConnection: keep-alive";
    char content_type[]   = "\r\nContent-Type: ";
    char content_length[] = "\r\nContent-Length: ";
    char date[] 		  = "\r\nDate: ";
    char server_name[]	  = "\r\nServer: Web Server di Lorenzo";
    char new_line[] 	  = "\r\n\r\n";

    char cLength[20];
    snprintf(cLength,sizeof(cLength), "%d",file_size);		// Content Length: convert int to string

    /**  
    *   Get current time
    *   The strftime() function formats the broken-down time tm (fourth parameter) according to
    *   the format specification format (third parameter) and places the result in the
    *   character array s (first parameter) of size max (second parameter).
    * */
    char currentTime[50];
    time_t now = time(0);
    struct tm data = *gmtime(&now);
    strftime(currentTime,sizeof(currentTime),"%a, %d %b %Y %H:%M:%S %Z", &data);

    //Allocate memory for the header string
    char *header = (char*)calloc(strlen(head) + strlen(keep_alive) + strlen(content_type) + strlen(media) + strlen(content_length) + strlen(cLength) + strlen(date) + strlen(currentTime) + strlen(server_name) + strlen(new_line) + 20, sizeof(char));

    //Fill the header string with all the fields.
    strcpy(header, head);
    strcat(header, content_type);
    strcat(header, media);
    strcat(header, content_length);
    strcat(header, cLength);
    strcat(header, keep_alive);
    strcat(header, date);
    strcat(header, currentTime);
    strcat(header, server_name);
    strcat(header, new_line);

    //Send the header message back to the client
    int bytes_send = send(socket, header, strlen(header), 0);
    if (bytes_send < 0) {
        perror("Error sending the header message");
        exit(EXIT_FAILURE);
    }

    //Free the allocated memory and return
    free(header);
    return bytes_send;
}


/**
 * This function gets the content type of the requested file from its extension.
 * @param path the path of the file
 * @return myme type of the file
 */
char* getContentType(char *path)
{
    /**
       The strrchr() function returns a pointer to the last occurrence of
       the character c in the string s. NULL if the character is not found.
    */
    char *dot = strrchr(path, '.');						
    char *extension;

    if(!dot || dot == path)
        extension = "";
    else
        extension = dot + 1;  //The variable "extension" points to the first character after the dot "."

    if(strncmp(extension, "html", 4) == 0 || strncmp(extension, "htm", 3) == 0)
        return "text/html";
    else if(strncmp(extension, "txt", 3) == 0)
        return "text/plain";
    else if(strncmp(extension, "jpeg", 4) == 0 || strncmp(extension, "jpg", 3) == 0)
        return "image/jpeg";
    else if(strncmp(extension, "JPEG", 4) == 0 || strncmp(extension, "JPG", 3) == 0)
        return "image/jpeg";
    else if(strncmp(extension, "PNG", 3) == 0 || strncmp(extension, "png", 3) == 0)
        return "image/png";
    else if(strncmp(extension, "gif", 3) == 0)
        return "image/gif";
    else if(strncmp(extension, "tiff", 4) == 0)
        return "image/tiff";
    else if(strncmp(extension, "pdf", 3) == 0)
        return "Application/pdf";
    else
        return "application/octet-stream";
}

/**
 * This function takes the complete http request 
 * and extracts the following data, returning them
 * in a httpFields struct pointer
 * Method
 * File
 * Version
 * User Agent
 * Accept Image Quality
 * @param httpRequest the complete http request
 * @return a pointer to httpFields struct
*/

httpFieldsPtr parseHttpRequest(char *httpRequest) {

    httpFieldsPtr resInfo = malloc(sizeof(struct httpFields));
    if (resInfo == NULL) {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }
    memset(resInfo, 0, sizeof(struct httpFields));

    printf("\nPARSE HTTP REQUEST:\n%s\n", httpRequest);
    strcpy(resInfo->method, strtok(httpRequest, " \t\n"));
    printf("RES INFO: %s\n", resInfo->method);
    strcpy(resInfo->file, strtok(NULL, " \t\n"));
    printf("RES INFO: %s\n", resInfo->file);
    strcpy(resInfo->version, strtok(NULL, " \t\n"));
    printf("RES INFO: %s\n", resInfo->version);

    return resInfo;
}

/**
 * This function takes the complete request and extracts the string
 * placed inside pattern1 and pattern 2
 * @param httpRequest: HTTP request string
 * @param pattern1: pattern 1
 * @param pattern2: pattern 2
 * @return substring between pattern1 and pattern2, NULL if there's none.
 */
char *extractSubString(char *httpRequest, char *pattern1, char *pattern2) {
    // Auxiliary vars
    const char *PATTERN1 = pattern1;
    const char *PATTERN2 = pattern2;
    char *start, *end, *target;

    start = strstr(httpRequest, PATTERN1);  // strstr returns a pointer to the substring PATTERN1 in the 
    if (start) {                            // httpRequest string , NULL if there's none.
        start += strlen(PATTERN1);
        end = strstr(start, PATTERN2);
        if (end) {
            target = malloc(end - start + 1);
            memcpy(target, start, end - start);
            target[end - start] = '\0';
            return target;
        }
    }
}

char *extracImageQualityAccept(char *acceptHeader){

    /**
     * Search for the keyword image in the accept header,
     * then search for the first q after the word "image",
     * then return the digits after q.
     * */
    const char *pattern = "image";
    const char *patternQ = "q=";
    char *start;
    char *qualityString;
    start = strstr(acceptHeader, pattern);
    if (start != NULL) {
        qualityString = strstr(start, patternQ);
        return qualityString;
    } else {
        return NULL;
    }
}

int *roundToClosestScalingFactor(float num) {
    static int closest[2] = {1,1};
    float possibleValues[] = {1, (float)7/(float)8, (float)3/(float)4, (float)5/(float)8, (float)1/(float)2, (float)3/(float)8, (float)1/(float)4, (float)1/(float)8};
    int numDenomPairs[8][2] = {{1,1}, {7,8}, {3,4}, {5,8}, {1,2}, {3,8}, {1,4}, {1,8}};
    for (int i = 0; i < 6; i++) {
        if ((num < possibleValues[i]) && (num > possibleValues[i+1])) {
            closest[0] = numDenomPairs[i][0];
            closest[1] = numDenomPairs[i][1];
            printf("Returning... %d %d\n", closest[0], closest[1]);
            return closest;
        } else if (num > 1) {
           printf("Returning... %d %d\n", closest[0], closest[1]);
            return closest;
        } else if (num < possibleValues[7]){  //Last case 1/8
            closest[0] = numDenomPairs[7][0];
            closest[1] = numDenomPairs[7][1];
            printf("Returning... %d %d\n", closest[0], closest[1]);
            return closest;
        }
    }
}

char *extractActualName(char *imgname) {
    char *actualName = strtok(imgname, "/");
    actualName = strtok(NULL, "/");
    actualName = strtok(NULL, "/");
    actualName = strtok(NULL, "/");
    return actualName;
}