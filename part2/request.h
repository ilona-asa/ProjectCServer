#ifndef __REQUEST_H__

#include "server_types.h"

long requestFileSize(int fd);

/* TODO: Add a parameter to assign a worker to each request */
void requestHandle(int fd, long arrival, long dispatch);

#endif
