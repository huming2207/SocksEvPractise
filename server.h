//
// Created by hu on 12/19/17.
//

#ifndef SOCKSEVPRACTISE_SERVER_H
#define SOCKSEVPRACTISE_SERVER_H

#include "common.h"


void server_init();
void server_action_cb(struct ev_loop * loop, ev_io * io_watcher, int revents);
void server_accept_cb(struct ev_loop * loop, ev_io * io_watcher, int revents);
void server_write_file_cb(struct ev_loop * loop, ev_periodic * timer_watcher, int revents);
void server_event_cleanup(struct ev_loop * loop, int fd);

#endif //SOCKSEVPRACTISE_SERVER_H
