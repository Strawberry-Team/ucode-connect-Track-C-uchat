#pragma once

//#include "socket.h"
//#include "threads.h"
//#include "database.h"
//#include "request_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BACKLOG 4

/**
 * @return nothing but turns server into the daemon state
*/
void create_deamon(void);

