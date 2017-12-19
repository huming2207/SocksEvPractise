//
// Created by hu on 12/19/17.
//

#ifndef SOCKSEVPRACTISE_SERVER_H
#define SOCKSEVPRACTISE_SERVER_H

#include "common.h"

struct ev_io * event_list[WORKING_CLIENT_COUNT] = {NULL};

void server_init();
void server_read_cb(struct ev_loop * loop, ev_io * io_watcher, int revents);
void server_accept_cb(struct ev_loop * loop, ev_io * io_watcher, int revents);
void server_write_file_cb(struct ev_loop * loop, ev_periodic * timer_watcher, int revents);



#endif //SOCKSEVPRACTISE_SERVER_H
