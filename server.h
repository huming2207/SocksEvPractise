//
// Created by hu on 12/19/17.
//

#ifndef SOCKSEVPRACTISE_SERVER_H
#define SOCKSEVPRACTISE_SERVER_H

#include <ev.h>


void server_init();

void server_action_cb(struct ev_loop *loop, ev_io *io_watcher, int revents);

void server_accept_cb(struct ev_loop *loop, ev_io *io_watcher, int revents);

void server_event_cleanup(struct ev_loop *loop, int fd);

void user_input_callback(struct ev_loop *loop, int revent);

void server_save_callback(struct ev_loop *loop, ev_io *watcher, int revent);

static unsigned long long send_counter;
static unsigned long long save_counter;

#endif //SOCKSEVPRACTISE_SERVER_H
