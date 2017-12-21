//
// Created by hu on 12/19/17.
//

#ifndef SOCKSEVPRACTISE_COMMON_H
#define SOCKSEVPRACTISE_COMMON_H

enum working_mode
{
  CLIENT_MODE,
  SERVER_MODE
};

#define WORKING_PORT            12000
#define WORKING_CLIENT_COUNT    512
#define STRING_BUFFER_SIZE      3072
#define SOCKS_LOG_FILE          "telnet.log"

#endif //SOCKSEVPRACTISE_COMMON_H
