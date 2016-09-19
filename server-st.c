/**
 * server-st.c: Simple, single-threaded web server implementation.
 *
 * Repeatedly handles HTTP requests sent to this port number.
 * Most of the work is done within routines written in request.c
 *
 * Course: 1DT032
 * 
 * To run:
 *  server <portnum (above 2000)>
 */

#include <assert.h>
#include "request.h"
#include "util.h"
#include <time.h> 

/* Structure of a HTTP request. */
typedef struct {
	int fd;
	long size, arrival, dispatch;
} request;

/**
 * Very simple input option parser.
 *
 * @argc: number of input arguments
 * @argv: input arguments
 * @port: port number that the server will listen.
 */
void getargs(int argc, char *argv[], int *port)
{
	/* TODO: Verify that the arguments are valid change by Asa*/
	assert(atoi(argv[1])!=0);     
    	if (argc != 2) 
	{
   	 	fprintf(stderr, "usage: %s <port>\n", argv[0]);
		
    		exit(1);
	}
	else
	{
		*port = atoi(argv[1]);
	}
	/*assert(argc >= 0);
	assert(argv != NULL);

	//assert(port >= 0);
	
	printf("argc: %d\n",argc);
	printf("argv: %s\n",*argv);
	*port = atoi(argv[1]);
	printf("port: %d\n",*port);
	
	// To verify the arguments from user is 2 arguments.
	if(argc == 2)
	{
		*port = atoi(argv[1]);
		printf("port: %d\n",*port);
	}
	else
	{	
		printf("Your Number of Argument not equal to 2\n");
	}
	
	/*if (argc != atoi(argv)) {
		printf("Error\n");
		/* number of input arguments is not the same with input arguments: kurang message helper*/
		/*exit(0);
	}else {
		return atoi(argv);
	}*/




int requestcmp(const void *first, const void *second) {
	assert(first != NULL);
	assert(second != NULL);
	return ((*(request **)first)->size - (*(request **)second)->size);
}
}

/**
 * Calculates the time in microsecond resultion.
 * 
 * ARGUMENTS:
 * @t: timeval structure
 */
long calculate_time(struct timeval t) {
	return ((t.tv_sec) * 1000 + t.tv_usec/1000.0) + 0.5;
}


/**
 * Main server loop
 */
int main(int argc, char *argv[]) {
	int listenfd, connfd, port, clientlen;
	struct sockaddr_in clientaddr;
	struct timeval arrival;
	time_t ticks;
	char req[MAXLINE];
	//struct request req;	

	/* Parse the input arguments */
	getargs(argc, argv, &port);

	printf("\n1DT032 server: Hello! I am running on port %d.\n\n", port);
	//printf("test1\n");
	/* Listen to user selected port */
	listenfd = Open_listenfd(port);
	
	//printf("istenfd %d\n", listenfd);

	/* Main server loop */
	while (1) {
		clientlen = sizeof(clientaddr);
		
		//Listen(port,LISTENQ);
		//printf("client address %d", clientlen);
		/* TODO: Accept a connection and retrieve connfd */
		 connfd = Accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
		if(connfd == 1)
		{
			printf("Good Job\n");
		}
		else
		{
			printf("Rahmanu Buat sampai selesai\n");
			break;
		}
	
		/* TODO: Allocate a request structure */
		request *st = malloc(sizeof(request));
		

		
		/* TODO: Save the time for the statistics */
		//gettimeofday(&arrival, NULL);
		struct timeval tvalbefore, tvalafter;
                gettimeofday(&tvalbefore, NULL);    
		
		
		/* TODO: Set the request file descriptor to the one accepted */
		
		st->fd = connfd;
		/*while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    		{
        		//Send the message back to client
        		write(client_sock , client_message , strlen(client_message));
    		}*/

		//snprintf(buf, MAXBUF, "%.24s\r\n", ctime(&ticks));
		//fd_set readfds, writefds, exceptfds;		
		//Select(connfd, &readfds, &writefds, &exceptfds, &arrival);
		

		/* TODO: Set the arrival and dispatch time */
		gettimeofday(&tvalafter, NULL);
                st->arrival = tvalbefore.tv_sec;
                st->dispatch = tvalafter.tv_sec;
		/*long arrive = calculate_time(arrival);
		long dispatch = calculate_time(arrival)+6000;
		printf("Test1,2,3");*/

		/* TODO: Call the request handler */
		requestHandle(st->fd, st->arrival, st->dispatch);
		printf("Test1,2");
		/* TODO: Close */
		Close(st->fd);
	}
return 0;
}


    


 
