//
// Created by hu on 12/19/17.
//
#include "server.h"
#include "common.h"

#include <stdio.h>
#include <errno.h>
#include <zconf.h>

#include <netinet/in.h>
#include <stdlib.h>


/**
 * Initialise the server mode
 *
 * Procedure:
 * socket ==> bind ==> listen ==> accept (event) ==> read & write (event) => close
 */
struct ev_io *event_list[WORKING_CLIENT_COUNT] = {NULL};
char *client_buffer;
char input_char;

void server_init()
{
  // Declare something
  struct ev_loop *loop = EV_DEFAULT;
  struct ev_io *socket_watcher;
  struct ev_io *input_watcher;
  struct sockaddr_in sock_addr;
  int socket_fd;
  int socket_opt_enable;

  // Initialise something
  loop = EV_DEFAULT;
  socket_watcher = malloc(sizeof(struct ev_io));
  input_watcher = malloc(sizeof(struct ev_io));

  // Prepare server address info, wipe it in case something weird happens
  memset(&sock_addr, 0, sizeof(struct sockaddr_in));

  if (!socket_watcher) {
    fprintf(stderr, "init: failed to allocate memory, reason: %s\n", strerror(errno));
  }

  // Initialise client buffer
  client_buffer = calloc(STRING_BUFFER_SIZE, sizeof(char));

  // Detect if malloc failed
  if (!client_buffer) {
    fprintf(stderr, "malloc: cannot allocate memory for client buffer: %s\n", strerror(errno));
    return;
  }


  // Register socket
  socket_fd = socket(PF_INET, SOCK_STREAM, 0);

  // If failed, exit.
  if (socket_fd < 0) {
    fprintf(stderr, "Socket: register failed, check your permission first.\n");
    exit(EXIT_FAILURE);
  }

  // Fill in the address
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(WORKING_PORT);
  sock_addr.sin_addr.s_addr = INADDR_ANY;

  // Here we go: start to bind
  if (bind(socket_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) != 0) {
    fprintf(stderr, "Bind: bind failed, try again later: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Here we go: start to listen
  if (listen(socket_fd, SOMAXCONN) == -1) {
    fprintf(stderr, "Listen: listen failed, try again later: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Reuse socket reference
  socket_opt_enable = 1;

#ifndef SO_REUSEPORT
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_opt_enable, sizeof(socket_opt_enable)) == -1){
      fprintf(stderr, "setsockopt: Reuse socket (SO_REUSEADDR) failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
  }
#else
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &socket_opt_enable, sizeof(socket_opt_enable)) == -1) {
    fprintf(stderr, "setsockopt: Reuse socket (SO_REUSEADDR) failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
#endif


  // Register socket event
  ev_io_init(socket_watcher, server_accept_cb, socket_fd, EV_READ);
  ev_io_start(loop, socket_watcher);

  // Register input event
  ev_io_init(input_watcher, user_input_callback, STDIN_FILENO, EV_READ);
  ev_io_start(loop, input_watcher);

  // Run the loop
  ev_run(loop, 0);

}

void server_accept_cb(struct ev_loop *loop, ev_io *io_watcher, int revents)
{
  // Declare something
  int client_fd;
  struct sockaddr_in client_addr;
  struct ev_io *client_watcher;

  socklen_t socket_size = sizeof(client_addr);

  // Initialise client event watcher
  client_watcher = malloc(sizeof(struct ev_io));

  // Detect if malloc failed
  if (!client_watcher) {
    fprintf(stderr, "malloc: cannot allocate memory for client event watcher: %s\n", strerror(errno));
    return;
  }

  // Detect if event register failed
  if (EV_ERROR & revents) {
    fprintf(stderr, "Event: cannot register the event: %s\n", strerror(errno));
    return;
  }

  // Accept the client
  client_fd = accept(io_watcher->fd, (struct sockaddr *) &client_addr, &socket_size);

  if (client_fd < 0) {
    fprintf(stderr, "Accept: cannot accept the client: %s\n", strerror(errno));
    return;
  }

  if (client_fd > WORKING_CLIENT_COUNT) {
    fprintf(stderr, "Accept: too many connections.\n");
    return;
  }

  // Initialise the action (r/w) event, listen, then read/write to client
  ev_io_init(client_watcher, server_action_cb, client_fd, EV_READ);
  ev_io_start(loop, client_watcher);

  event_list[client_fd] = client_watcher;
}

void server_action_cb(struct ev_loop *loop, ev_io *io_watcher, int revents)
{

  ssize_t read_fd;
  ssize_t send_fd;

  // Detect if event register failed
  if (EV_ERROR & revents) {
    fprintf(stderr, "Event: cannot register the event\n");
    return;
  }

  // Here we go, start to read.
  read_fd = recv(io_watcher->fd, client_buffer, STRING_BUFFER_SIZE, 0);

  if (read_fd < 0) {
    fprintf(stderr, "Recv: data receive failed, reason: %s\n", strerror(errno));
    free(client_buffer);
    return;
  } else if (read_fd == 0) {
    fprintf(stderr, "Recv: client disconnected.\n");
    server_event_cleanup(loop, io_watcher->fd);
    return;
  }

  // Send the echo back to client user
  send_fd = send(io_watcher->fd, client_buffer, STRING_BUFFER_SIZE, 0);

  if (send_fd < 0) {
    fprintf(stderr, "Send: data send failed, reason: %s\n", strerror(errno));
    free(client_buffer);
    return;
  } else if (send_fd == 0) {
    fprintf(stderr, "Send: client disconnected!\n");
    server_event_cleanup(loop, io_watcher->fd);
    return;
  }

  send_counter += 1;

}


void server_event_cleanup(struct ev_loop *loop, int ref)
{
  // If this item has been freed, stop here.
  if (!event_list[ref]) {
    printf("free: client has been freed.\n");
    return;
  }

  // Free up memory
  ev_io_stop(loop, event_list[ref]);
  event_list[ref] = NULL;
  free(event_list[ref]);
  close(ref);

}

void user_input_callback(struct ev_loop *loop, int revent)
{

  input_char = (char)fgetc(stdin);

  switch(input_char)
  {
    case 's': {
      printf("[Stat] Already sent %lu times!\n", send_counter);
      break;
    }

    default: {
      break;
    }
  }
}