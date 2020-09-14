
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
#include <math.h>

// Headers
#include "httpHandler.h"

// Global Variables

int cacheDim = 10;                           //La dimensione è espressa come numero di file presenti, dim = 0 --> Cache Disabilitata
int isCacheEnabled = 1;                      //Di default la cache è attiva con dimensione 10
int portNumber = 8080;                       //La porta di default è la 8080
struct sockaddr_in server_addr, client_addr; //Strutture per server address e client address
int list_socket, conn_socket;                //Listening socket e Connection socket
socklen_t client_len;
int result_bytes;


// Thread parameters
struct pthread_param{
    int socket;
    char ip_address[16];
    pthread_t tid;
};

// Prototypes

void parseCommandLine(int argc, char const *argv[]);
void handleClientConnection(void *params);