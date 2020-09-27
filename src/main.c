#include "main.h"

#define BACKLOG 10000
#define MAX_BYTES 4096

/* ---- JPEG IMAGE TEST ----- */



int main(int argc, char const *argv[])
{

    /* Launching the program, you can choose if the cache is enabled or not
    *  and which port the server should use
    *  Usage: ./server [-p portNumber] [-c dim]
    *  portNumber specifies the port number to use
    *  dim sets the cache dimension.
    *  Setting dim to 0 disables the cache.
    */

    parseCommandLine(argc, argv);

    printf("Port number: %d\n", portNumber);
    printf("Cache: %d, Dim: %d\n", isCacheEnabled, cacheDim);
    

	/*	Server socket
	*	Uses IPv4 addresses over TCP.
	*/
    list_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (list_socket < 0)
    {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

	
		
	//Prepare the server data structure to hold server informations.
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNumber);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //Bind
    if (bind(list_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    //Listen
    int listenStatus = listen(list_socket, BACKLOG);
    if (listenStatus < 0)
    {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        /*  Accept
        *   Accept an incoming connection from the queue of the listening socket
        */

        //Prepare the data structure to hold the client informations
        bzero((char *) &client_addr, sizeof(client_addr));
        client_len = sizeof(client_addr);

        //Accept call
        printf("Accepting\n");
        conn_socket = accept(list_socket, (struct sockaddr *) &client_addr, &client_len);
        printf("Accepted\n");
        if (conn_socket < 0) {
            perror("Accept error");
            exit(EXIT_FAILURE);
        }

        //Hold the client information into the client_pt struct
        struct sockaddr_in *client_pt = (struct sockaddr_in *) &client_addr;
        struct in_addr ip_addr = client_pt->sin_addr;
        char client_ip_addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_addr, client_ip_addr, INET_ADDRSTRLEN);

        /*  Thread
        *   Thread initialization and creation.
        *   The threads handles the connection with the client
        *   and responds to it.
        * */

        pthread_t tid;
        struct pthread_param *param = malloc(sizeof(struct pthread_param));  //Thread parameters
        param->socket = conn_socket;
        strcpy(param->ip_address, client_ip_addr);                           //Client ip address

        if (pthread_create(&(param->tid), NULL, (void *) handleClientConnection, param))
        {
            perror("Errore nella creazione del thread");
            exit(EXIT_FAILURE);
        }
        
    }
    
    return 0;
}

void parseCommandLine(int argc, char const *argv[])
{

    int params = 1;
    const char usage[50] = "Usage: ./server [-p portNumber] [-c dim]";
    const char portError[70] = "Ports from 0 to 1023 are reserved and cannot be used";

    if (params > argc)
    {
        printf("%s", usage);
        exit(EXIT_FAILURE);
    }

    for (params; params < argc; params++)
    {

        //PORT NUMBER
        if (strcmp(argv[params], "-p") == 0)
        {
            params++;
            if (params < argc)
            {
                if (isANumber(argv[params]))
                {
                    portNumber = atoi(argv[params]);
                    if (portNumber < 1024)
                    {
                        printf("%s\n", portError);
                        printf("%s\n", usage);
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    printf("Port number is not valid\n %s\n", usage);
                    exit(EXIT_FAILURE);
                }
            }
        }

        //CACHE
        else if (strcmp(argv[params], "-c") == 0)
        {
            params++;
            if (params < argc)
            {
                if (isANumber(argv[params]))
                {
                    cacheDim = atoi(argv[params]);
                    if (cacheDim == 0)
                    {
                        isCacheEnabled = 0;
                        continue;
                    }
                    else
                        continue;
                }
                else
                {
                    printf("Cache dimension is not valid\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                printf("%s\n", usage);
                exit(EXIT_FAILURE);
            }
        }

        else
        {
            printf("Invalid arguments\n");
            printf("%s\n", usage);
            exit(EXIT_FAILURE);
        }
    }
}

void handleClientConnection(void *thread_params) {


    // Recasting parameters to the correct data type
    struct pthread_param *params = (struct pthread_param *) thread_params;
    printf("Client connesso: %s on socket: %d, Thread ID: %ld\n", params->ip_address, params->socket, params->tid);

    /**
     * Set a recving timeout to 30 seconds.
     * The connections is still open, and the client can send new data through
     * the same socket without enstablishing a new connection.
     * SO_RCVTIMEO sets a timeout for an input operation. Default is to not wait
     * for an input operation. After the timeout if the same client makes a 
     * new request, a new socket is created and a new connection is enstablished.
    */

    struct timeval timeval;
    timeval.tv_sec = 30;
    int result = setsockopt(params->socket, SOL_SOCKET,SO_RCVTIMEO, (const char*)&timeval,sizeof(timeval));
    if(result == -1){
        perror("Cannot set RECV Timeout ");
        pthread_exit((void*)EXIT_FAILURE);
    }

    /**
     * Recieve from client.
     * Data transferred from the socket ends in "buffer"
    */
    int btyes_recvd;
    char buffer[4096];
    bzero(buffer, 4096);
    btyes_recvd = recv(params->socket, buffer, MAX_BYTES, 0);  //With 0 flags recv == read
    if (btyes_recvd <= 0) {
        perror("Error during request from client");
        pthread_exit((void *)EXIT_FAILURE);
    }

    /**
     * While loop to manage the client requests.
     * The loop ends when there's an error in recieving from client (bytes_recvd < 0)
     * or the client is not making requests anymore (bytes_recvd == 0, timeout)
    */

    while (btyes_recvd > 0) {

        /**
         * Extract the User Agent string from the request
         * and the accept image quality value.
        */
        char *userAgent = extractSubString(buffer, "User-Agent: ", "\n");
        char *acceptHeader = extractSubString(buffer, "Accept: ", "\n");
        char *qualityString = extracImageQualityAccept(acceptHeader);
        float quality;
        if (qualityString == NULL) {
            quality = 1.0;
        } else {
            quality = strtof(qualityString+2, NULL);
        }
        //printf("QUALITY: %f\n", quality);

        /**
         * Parse the http request to extract:
         * Method
         * Requested file
         * Http version
        */
        httpFieldsPtr httpFieldsParsed = (httpFieldsPtr) malloc(sizeof(struct httpFields));
        if (httpFieldsParsed == NULL) {
            perror("Unable to allocate memory");
            exit(EXIT_FAILURE);
        }
        httpFieldsParsed = parseHttpRequest(buffer);   // Function parseHttpRequest --> utils.c

        int request_type = requestType(httpFieldsParsed->method);

        /*
        *   Different actions based on the method.
        *   GET --> handleGETRequest()
        *   HEAD --> handleHEADRequest()
        * */
        switch (request_type)
        {
            //GET
            case 1:
                result_bytes = handleGETRequest(params->socket, httpFieldsParsed->file, userAgent, quality);
                if (result_bytes > 0) {
                    printf("Sent succesfully to client: %s on socket: %d\n\n\n", params->ip_address, params->socket);
                } else {
                    perror("Error handling GET request");
                    pthread_exit((void *)EXIT_FAILURE);
                }
                break;

            //HEAD
            case 2:
                handleHEADRequest();
                break;

            //POST
            case 3:
                //handlePOSTRequest();
                break;

            default:
                sendErrorMessage(params->socket, 400);
                break;

        }

        bzero(buffer, MAX_BYTES);
        btyes_recvd = recv(params->socket, buffer, sizeof(buffer), 0);        // Recieve next request from Client

    }


    if (btyes_recvd < 0) { 
        // Error in recieving from client
        close(params->socket);
        pthread_exit((void *)EXIT_FAILURE);

    } else if (btyes_recvd == 0) {
        // Timeout-No bytes recieved
        printf("TIMEOUT no bytes recieved\n"); 
        close(params->socket);
        pthread_exit((void *)EXIT_SUCCESS);
    }
}