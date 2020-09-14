#include "errorHandler.h"

/**
 * This function sends an appropriate error message to 
 * the client, depending on the event that caused the error.
 * @param socket the socket where to send the error message
 * @param code the status code of the error (404, 403 ecc.)
*/

void sendErrorMessage(int socket, int code) {

    char errorMessage[512];

    //For timestamp
    char currentTime[50];
    time_t now = time(0);
    struct tm data = *gmtime(&now);
    strftime(currentTime,sizeof(currentTime),"%a, %d %b %Y %H:%M:%S %Z", &data);


    switch (code)
    {
       case 400: 
            snprintf(errorMessage, sizeof(errorMessage), "HTTP/1.1 400 Bad Request\r\nContent-Length: 95\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nDate: %s\r\nServer: Web Server di Lorenzo/14785\r\n\r\n<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\n<BODY><H1>400 Bad Rqeuest</H1>\n</BODY></HTML>", currentTime);
            send(socket, errorMessage, strlen(errorMessage), 0);
            break;

        case 403: 
            snprintf(errorMessage, sizeof(errorMessage), "HTTP/1.1 403 Forbidden\r\nContent-Length: 112\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nDate: %s\r\nServer: Web Server di Lorenzo/14785\r\n\r\n<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\n<BODY><H1>403 Forbidden</H1><br>Permission Denied\n</BODY></HTML>", currentTime);
            send(socket, errorMessage, strlen(errorMessage), 0);
            break;

        case 404: 
            snprintf(errorMessage, sizeof(errorMessage), "HTTP/1.1 404 Not Found\r\nContent-Length: 91\r\nContent-Type: text/html\r\nConnection: keep-alive\r\nDate: %s\r\nServer: Web Server di Lorenzo/14785\r\n\r\n<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\n<BODY><H1>404 Not Found</H1>\n</BODY></HTML>", currentTime);
            send(socket, errorMessage, strlen(errorMessage), 0);
            break;

        case 500: 
            snprintf(errorMessage, sizeof(errorMessage), "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 115\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nDate: %s\r\nServer: Web Server di Lorenzo/14785\r\n\r\n<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>\n<BODY><H1>500 Internal Server Error</H1>\n</BODY></HTML>", currentTime);
            send(socket, errorMessage, strlen(errorMessage), 0);
            break;

        case 501: 
            snprintf(errorMessage, sizeof(errorMessage), "HTTP/1.1 501 Not Implemented\r\nContent-Length: 103\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nDate: %s\r\nServer: Web Server di Lorenzo/14785\r\n\r\n<HTML><HEAD><TITLE>501 Not Implemented</TITLE></HEAD>\n<BODY><H1>501 Not Implemented</H1>\n</BODY></HTML>", currentTime);
            send(socket, errorMessage, strlen(errorMessage), 0);
            break;

        case 505: 
            snprintf(errorMessage, sizeof(errorMessage), "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Length: 125\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nDate: %s\r\nServer: Web Server di Lorenzo/14785\r\n\r\n<HTML><HEAD><TITLE>505 HTTP Version Not Supported</TITLE></HEAD>\n<BODY><H1>505 HTTP Version Not Supported</H1>\n</BODY></HTML>", currentTime);
            send(socket, errorMessage, strlen(errorMessage), 0);
            break;

        default:
            //Case 500
            snprintf(errorMessage, sizeof(errorMessage), "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 115\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nDate: %s\r\nServer: Web Server di Lorenzo/14785\r\n\r\n<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>\n<BODY><H1>500 Internal Server Error</H1>\n</BODY></HTML>", currentTime);
            send(socket, errorMessage, strlen(errorMessage), 0);
            break;

    }

}