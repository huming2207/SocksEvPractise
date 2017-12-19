//
// Created by hu on 12/19/17.
//

#ifndef SOCKSEVPRACTISE_COMMON_H
#define SOCKSEVPRACTISE_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ev.h>
#include <errno.h>

enum working_mode {
    CLIENT_MODE,
    SERVER_MODE
};

#define WORKING_PORT            12000
#define WORKING_CLIENT_COUNT    512
#define STRING_BUFFER_SIZE      3072
#define SOCKS_LOG_FILE          "telnet.log"




#endif //SOCKSEVPRACTISE_COMMON_H
