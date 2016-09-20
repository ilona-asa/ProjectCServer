     
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
    	/* TODO: Verify that the arguments are valid */
        assert(atoi(argv[1])!=0);     
        if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
      }
        *port = atoi(argv[1]);
     
    int requestcmp(const void *first, const void *second) {
    	assert(first != NULL);
    	assert(second != NULL);
    	return ((*(request **)first)->size - (*(request **)second)->size);
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
     
    	/* Parse the input arguments */
    	getargs(argc, argv, &port);
     
    	printf("\n1DT032 server: Hello! I am running on port %d.\n\n", port);
     
    	/* Listen to user selected port */
    	listenfd = Open_listenfd(port);
     
    	/* Main server loop */
    	while (1) {
    		clientlen = sizeof(clientaddr);
     
    		/* TODO: Accept a connection and retrieve connfd */
     
            connfd = accept(listenfd, &clientaddr, &clientlen);
            if (connfd < 0)
            error("ERROR on accept");
     
     
     
    		/* TODO: Allocate a request structure */
                request *st = malloc(sizeof(request));
     
    		/* TODO: Save the time for the statistics */
                  struct timeval tvalbefore, tvalafter;
                   gettimeofday(&tvalbefore, NULL);
    		/* TODO: Set the request file descriptor to the one accepted 
                      st->fd = connfd;
    		/* TODO: Set the arrival and dispatch time */
    		    gettimeofday(&tvalafter, NULL);
                    st->arrival = tvalbefore.tv_sec;
                    st->dispatch = tvalafter.tv_sec;
     
    		/* TODO: Call the request handler */
                      requestHandle(st->fd, st->arrival, st->dispatch);
     
              /* DONE: Close */
     
               Close(st->fd);
               exit(0);
     
     
    	}
     
    }
     
     
     
    /**
     * request.c: Does the bulk of the work for the web server.
     *
     * Course: 1DT032
     * 
     */
     
    #include <assert.h>
    #include "util.h"
    #include "request.h"
     
    /**
     * requestError
     *
     * ARGUMENTS:
     * @fd:
     * @cause:
     * @errnum:
     * @shortmsg:
     * @longmsg:
     */
    void requestError(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    	assert(cause != NULL);
    	assert(errnum != NULL);
    	assert(shortmsg != NULL);
    	assert(longmsg != NULL);
     
    	char buf[MAXLINE], body[MAXBUF];
     
    	printf("Request ERROR\n");
     
    	/* Create the body of the error message */
    	sprintf(body, "<html><title>1DT032 Error</title>");
    	sprintf(body, "%s<body bgcolor=""fffff"">\r\n", body);
    	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    	sprintf(body, "%s<hr>1DT032 Web Server\r\n", body);
     
    	/* Write out the header information for this response */
    	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    	Rio_writen(fd, buf, strlen(buf));
    	printf("%s", buf);
     
    	sprintf(buf, "Content-Type: text/html\r\n");
    	Rio_writen(fd, buf, strlen(buf));
    	printf("%s", buf);
     
    	sprintf(buf, "Content-Length: %lu\r\n\r\n", strlen(body));
    	Rio_writen(fd, buf, strlen(buf));
    	printf("%s", buf);
     
    	/* Write out the content */
    	Rio_writen(fd, body, strlen(body));
    	printf("%s", body);
    }
     
     
    /**
     * Reads and discards everything up to an empty text line
     */
    void requestReadHdrs(rio_t *rp) {
    	assert(rp != NULL);
     
    	char buf[MAXLINE];
        char tmp[MAXLINE];
     
        Rio_readlineb(rp, buf, MAXLINE);
     
        int riosize;
        while((riosize = Rio_readlineb(rp, tmp, MAXLINE)) > 2) {
          printf("riosize: %d\n", riosize);
        }
     
    	/* DONE: 
    	 * The previous line will only read one line, however, it should
    	 * discard everything up to an empty text line.
    	 */
     
    	return;
    }
     
    /**
     * Parses the request URI.
     * Calculates @filename (and @cgiargs for dynamic) from @uri.
     *
     * RETURNS: 1 if static, 0 if dynamic content.
     */
    int requestParseURI(char *uri, char *filename, char *cgiargs) {
    	assert(uri != NULL);
    	assert(filename != NULL);
    	assert(cgiargs != NULL);
     
    	char *ptr;
     
    	if (!strstr(uri, "cgi")) {
    		/* Static */
    		strcpy(cgiargs, "");
    		sprintf(filename, ".%s", uri);
    		if (uri[strlen(uri)-1] == '/') {
    			strcat(filename, "home.html");
    		}
    		return 1;
    	} else {
    		/* Dynamic */
    		ptr = index(uri, '?');
    		if (ptr) {
    			strcpy(cgiargs, ptr+1);
    			*ptr = '\0';
    		} else {
    			strcpy(cgiargs, "");
    		}
    		sprintf(filename, ".%s", uri);
    		return 0;
    	}
    }
     
    /**
     * Fills in the @filetype given the @filename.
     */
    void requestGetFiletype(char *filename, char *filetype) {
    	assert(filename != NULL);
    	assert(filetype != NULL);
     
    	if (strstr(filename, ".html")) {
    		strcpy(filetype, "text/html");
    	} else if (strstr(filename, ".gif")) {
    		strcpy(filetype, "image/gif");
    	} else if (strstr(filename, ".jpg")) {
    		strcpy(filetype, "image/jpeg");
    	} else { 
    		strcpy(filetype, "test/plain");
    	}
    }
     
    /**
     * Handles a dynamic request.
     */
    void requestServeDynamic(int fd, char *filename, char *cgiargs, long arrival, long dispatch) {
    	assert(filename != NULL);
    	assert(cgiargs != NULL);
     
    	char buf[MAXLINE], *emptylist[] = {NULL};
     
    	/* The server does only a little bit of the header.
    	 * The CGI script has to finish writing out the header. */
    	sprintf(buf, "HTTP/1.0 200 OK\r\n");
    	sprintf(buf, "%sServer: 1DT032 Web Server\r\n", buf);
           	sprintf(buf, "%s Stat-req-arrival: %ld\r\n", buf, arrival);
    	sprintf(buf, "%s Stat-req-dispatch: %d\r\n", buf, (int)(dispatch - arrival));
     
    	Rio_writen(fd, buf, strlen(buf));
     
    	if (Fork() == 0) {
    		/* Child process */
    		Setenv("QUERY_STRING", cgiargs, 1);
    		/* When the CGI process writes to stdout, it will instead go to the socket */
    		Dup2(fd, STDOUT_FILENO);
    		Execve(filename, emptylist, environ);
    	}
    	Wait(NULL);
    }
     
     
    /**
     * Handles a static request.
     * Responds to @fd, sending @filesize bytes from @filename.
     */
    void requestServeStatic(int fd, char *filename, int filesize, long arrival, long dispatch) {
    	assert(filename != NULL);
     
    	int srcfd;
    	char *srcp, filetype[MAXLINE], buf[MAXBUF];
    	struct timeval beforeread, afterread;
    	int read, complete;
     
    	gettimeofday(&beforeread, NULL);
     
    	requestGetFiletype(filename, filetype);
     
    	srcfd = Open(filename, O_RDONLY, 0);
     
    	/* Rather than call read() to read the file into memory, 
    	   which would require that we allocate a buffer, we memory-map the file */
    	srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    	Close(srcfd);
     
    	gettimeofday(&afterread, NULL);
     
    	read = ((afterread.tv_sec - beforeread.tv_sec) * 1000 + (afterread.tv_usec - beforeread.tv_usec)/1000.0) + 0.5;
    	complete = (((afterread.tv_sec) * 1000 + (afterread.tv_usec)/1000.0) + 0.5) - arrival;
     
    	/* Put together response */
    	sprintf(buf, "HTTP/1.0 200 OK\r\n");
    	sprintf(buf, "%sServer: 1DT032 Web Server\r\n", buf);
    	sprintf(buf, "%s Stat-req-arrival: %ld\r\n", buf, arrival);
    	sprintf(buf, "%s Stat-req-dispatch: %d\r\n", buf, (int)(dispatch - arrival));
    	sprintf(buf, "%s Stat-req-read: %d\r\n", buf, read);
    	sprintf(buf, "%s Stat-req-complete: %d\r\n", buf, complete); 
     
    	sprintf(buf, "%sContent-Length: %d\r\n", buf, filesize);
    	sprintf(buf, "%sContent-Type: %s\r\n\r\n", buf, filetype);
     
    	Rio_writen(fd, buf, strlen(buf));
     
    	/*  Writes out to the client socket the memory-mapped file */
    	Rio_writen(fd, srcp, filesize);
    	Munmap(srcp, filesize);
     
    }
     
     
    /**
     * Receives and handles a request from @fd, setting the
     * @arrival and @dispatch times for the statistics.
     */
    void requestHandle(int fd, long arrival, long dispatch) {
    	int is_static;
    	struct stat sbuf;
    	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    	char filename[MAXLINE], cgiargs[MAXLINE];
    	rio_t rio;
     
    	Rio_readinitb(&rio, fd);
    	Rio_readlineb(&rio, buf, MAXLINE);
    	sscanf(buf, "%s %s %s", method, uri, version);
     
    	printf("%s %s %s\n", method, uri, version);
     
    	/* DONE: Return an error if a GET method is received. */
     
    	/* Read request headers */
            if (strcmp(method, "GET") != 0) {
              requestError(fd, filename, "501", "Not Implemented",
                           "1DT032 Server has not implemented this method");
              return;
     
            }
    	requestReadHdrs(&rio);
     
    	/* Parse the URI and check for static request */
    	is_static = requestParseURI(uri, filename, cgiargs);
     
    	/* Check if file is present in the directory */
    	if (stat(filename, &sbuf) < 0) {
    		/* The file requested is missing. Produce an error */
    		requestError(fd, filename, "404", "Not found", 
    			     "1DT032 Server could not find this file");
    		return;
    	}
     
    	if (is_static) {
    		/* Static request */
    		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
    			requestError(fd, filename, "403", "Forbidden", "1DT032 Server could not read this file");
    			return;
    		}
    		/* Delegate the request processing to the Request module */
    		requestServeStatic(fd, filename, sbuf.st_size, arrival, dispatch);
    	} else {
    		/* DONE: Implement the dynamic case */
            if (!(S_IRUSR & sbuf.st_mode) || !(S_IXUSR & sbuf.st_mode)) {
                requestError(fd, filename, "403", "Forbidden", "1DT032 Server could not execute this file");
                return;
            }
     
     
    		/* Delegate the request processing to the Request module */
    		requestServeDynamic(fd, filename, cgiargs, arrival, dispatch);
    	}
    }
     
     
    /**
     * Returns the total size of a request.
     */
    long requestFileSize(int fd) {
    	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    	char filename[MAXLINE], cgiargs[MAXLINE];	
    	struct stat s;
     
    	/* Receive some bytes from the client */
    	recv(fd, buf, sizeof(buf), MSG_PEEK);
     
    	/* Parse method, uri and version from buf */
    	sscanf(buf, "%s %s %s\n", method, uri, version);
     
    	/* Parse URI */
    	requestParseURI(uri, filename, cgiargs);
     
    	Stat(filename, &s);
     
    	return (long)s.st_size;
    }
     
    #ifndef __REQUEST_H__
     
    long requestFileSize(int fd);
     
    void requestHandle(int fd, long arrival, long dispatch);
     
    #endif
     
    #ifndef __CSAPP_H__
    #define __CSAPP_H__
     
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <string.h>
    #include <strings.h>
    #include <ctype.h>
    #include <setjmp.h>
    #include <signal.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <errno.h>
    #include <math.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
     
    /* Default file permissions are DEF_MODE & ~DEF_UMASK */
    #define DEF_MODE   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
    #define DEF_UMASK  S_IWGRP|S_IWOTH
     
    /* Simplifies calls to bind(), connect(), and accept() */
    typedef struct sockaddr SA;
     
    /* Persistent state for the robust I/O (Rio) package */
     
    #define RIO_BUFSIZE 8192
    typedef struct {
        int rio_fd;                /* descriptor for this internal buf */
        int rio_cnt;               /* unread bytes in internal buf */
        char *rio_bufptr;          /* next unread byte in internal buf */
        char rio_buf[RIO_BUFSIZE]; /* internal buffer */
    } rio_t;
     
    /* External variables */
    extern int h_errno;    /* defined by BIND for DNS errors */ 
    extern char **environ; /* defined by libc */
     
    /* Misc constants */
    #define MAXLINE  8192  /* max text line length */
    #define MAXBUF   8192  /* max I/O buffer size */
    #define LISTENQ  1024  /* second argument to listen() */
     
    /* Our own error-handling functions */
    void unix_error(char *msg);
    void posix_error(int code, char *msg);
    void dns_error(char *msg);
    void app_error(char *msg);
     
    /* Process control wrappers */
    pid_t Fork(void);
    void Execve(const char *filename, char *const argv[], char *const envp[]);
    pid_t Wait(int *status);
     
    int Gethostname(char *name, size_t len) ;
    int Setenv(const char *name, const char *value, int overwrite);
     
    /* Unix I/O wrappers */
    int Open(const char *pathname, int flags, mode_t mode);
    ssize_t Read(int fd, void *buf, size_t count);
    ssize_t Write(int fd, const void *buf, size_t count);
    off_t Lseek(int fildes, off_t offset, int whence);
    void Close(int fd);
    int Select(int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, 
               struct timeval *timeout);
    int Dup2(int fd1, int fd2);
    void Stat(const char *filename, struct stat *buf);
    void Fstat(int fd, struct stat *buf) ;
     
    /* Memory mapping wrappers */
    void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
    void Munmap(void *start, size_t length);
     
    /* Sockets interface wrappers */
    int Socket(int domain, int type, int protocol);
    void Setsockopt(int s, int level, int optname, const void *optval, int optlen);
    void Bind(int sockfd, struct sockaddr *my_addr, int addrlen);
    void Listen(int s, int backlog);
    int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
    void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
     
    /* DNS wrappers */
    struct hostent *Gethostbyname(const char *name);
    struct hostent *Gethostbyaddr(const char *addr, int len, int type);
     
    /* Rio (Robust I/O) package */
    ssize_t rio_readn(int fd, void *usrbuf, size_t n);
    ssize_t rio_writen(int fd, void *usrbuf, size_t n);
    void rio_readinitb(rio_t *rp, int fd); 
    ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);
    ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
     
    /* Wrappers for Rio package */
    ssize_t Rio_readn(int fd, void *usrbuf, size_t n);
    void Rio_writen(int fd, void *usrbuf, size_t n);
    void Rio_readinitb(rio_t *rp, int fd); 
    ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n);
    ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
     
    /* Client/server helper functions */
    int open_clientfd(char *hostname, int portno);
    int open_listenfd(int portno);
     
    /* Wrappers for client/server helper functions */
    int Open_clientfd(char *hostname, int port);
    int Open_listenfd(int port); 
     
    #endif /* __CSAPP_H__ */
     
    /*
     * client.c: A very, very primitive HTTP client.
     * 
     * Sends one HTTP request to the specified HTTP server.
     * Prints out the HTTP response.
     * For testing your server, you might want to modify this client.
     *
     * Course: 1DT032   
     *
     */
     
    #include "util.h"
     
    /*
     * Send an HTTP request for the specified file 
     */
    void clientSend(int fd, char *filename)
    {
    	char buf[MAXLINE];
    	char hostname[MAXLINE];
     
    	Gethostname(hostname, MAXLINE);
     
    	/* Form and send the HTTP request */
    	sprintf(buf, "GET %s HTTP/1.1\n", filename);
    	sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    	Rio_writen(fd, buf, strlen(buf));
    }
     
    /*
     * Read the HTTP response and print it out
     */
    void clientPrint(int fd, int print_content)
    {
    	rio_t rio;
    	char buf[MAXBUF];  
    	int length = 0;
    	int n;
     
    	Rio_readinitb(&rio, fd);
     
    	/* Read and display the HTTP Header */
    	n = Rio_readlineb(&rio, buf, MAXBUF);
    	while (strcmp(buf, "\r\n") && (n > 0)) {
    		printf("Header: %s", buf);
    		n = Rio_readlineb(&rio, buf, MAXBUF);
     
    		/* If you want to look for certain HTTP tags... */
    		if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
    			printf("Length = %d\n", length);
    		}
    	}
    	printf("\nContent:\n");
     
     
    	/* Read and display the HTTP Body */
    	n = Rio_readlineb(&rio, buf, MAXBUF);
    	while (n > 0) {
    		if (print_content) {
    			printf("%s", buf);
    		}
    		n = Rio_readlineb(&rio, buf, MAXBUF);
    	}
    }
     
    int main(int argc, char *argv[])
    {
    	char *host = NULL, *filename = NULL;
    	int port = 0;
    	int clientfd = 0;
    	int print_content = 0;
     
    	if (argc != 5) {
    		fprintf(stderr, "Usage: %s <host> <port> <filename> <print_content [yes/no]>\n", argv[0]);
    		exit(1);
    	} 
     
    	if (strcasecmp(argv[4], "yes") == 0) {
    		print_content = 1;
    	} else if (strcasecmp(argv[4], "no") == 0) {
    		print_content = 0;
    	} else {
    		fprintf(stderr, "Print content option must be 'yes' or 'no'\n");
    		exit(1);
    	}
     
    	host = argv[1];
    	port = atoi(argv[2]);
    	filename = argv[3];
     
     
    	/* Open a single connection to the specified host and port */
    	clientfd = Open_clientfd(host, port);
     
    	clientSend(clientfd, filename);
    	clientPrint(clientfd, print_content);
     
    	Close(clientfd);
     
    	exit(0);
    }
     
     
     
     
     
     
     
     
     
