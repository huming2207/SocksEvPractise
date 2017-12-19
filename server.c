//
// Created by hu on 12/19/17.
//

#include "server.h"

/**
 * Initialise the server mode
 *
 * Procedure:
 * socket ==> bind ==> listen ==> accept (event) ==> read & write (event) => close
 */
struct ev_io * event_list[WORKING_CLIENT_COUNT] = {NULL};

void server_init(){

    // Create a default event loop
    struct ev_loop *loop = EV_DEFAULT;

    // Initialise watchers
    struct ev_io *socket_watcher = (struct ev_io*)malloc(sizeof(struct ev_io));
    struct ev_periodic *periodic_watcher = (struct ev_periodic*)malloc(sizeof(struct ev_periodic));

    // Register socket
    int socket_ref = socket(PF_INET, SOCK_STREAM, 0);

    // If failed, exit.
    if(socket_ref < 0) {
        fprintf(stderr, "Socket: register failed, check your permission first.\n");
        exit(EXIT_FAILURE);
    }


    // Prepare server address info, wipe it in case something weird happens
    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(struct sockaddr_in));

    // Fill in the address
    sock_addr.sin_family      = AF_INET;
    sock_addr.sin_port        = htons(WORKING_PORT);
    sock_addr.sin_addr.s_addr = INADDR_ANY;

    // Here we go: start to bind
    if(bind(socket_ref, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) != 0){
        fprintf(stderr, "Bind: bind failed, try again later: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Here we go: start to listen
    if(listen(socket_ref, SOMAXCONN) != 0){
        fprintf(stderr, "Listen: listen failed, try again later: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Reuse socket reference
#ifndef SO_REUSEPORT
    if(setsockopt(socket_ref, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        fprintf(stderr, "setsockopt: Reuse socket (SO_REUSEADDR) failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
#else
    if(setsockopt(socket_ref, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0){
        fprintf(stderr, "setsockopt: Reuse socket (SO_REUSEADDR) failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
#endif

    // Allocate memory for save_buffer
    if(!(save_buffer = malloc(STRING_BUFFER_SIZE))){
        fprintf(stderr, "save_buffer: cannot allocate memory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Register periodic logging events
    ev_periodic_init(periodic_watcher, server_write_file_cb, 0., 5., 0);
    ev_periodic_start(loop, periodic_watcher);

    // Register socket events
    ev_io_init(socket_watcher, server_accept_cb, socket_ref, EV_READ);
    ev_io_start(loop, socket_watcher);

    // Endlessly gets user query
    while(true)
        ev_run(loop, 0);

}

void server_accept_cb(struct ev_loop * loop, ev_io * io_watcher, int revents){

    // Declare client user's address
    struct sockaddr_in client_addr;
    socklen_t socket_size = sizeof(client_addr);


    // Initialise client event watcher
    struct ev_io * client_watcher = (struct ev_io *)malloc(sizeof(struct ev_io));

    // Detect if malloc failed
    if(!client_watcher){
        fprintf(stderr, "malloc: cannot allocate memory for client event watcher: %s\n", strerror(errno));
        return;
    }

    // Detect if event register failed
    if (EV_ERROR & revents) {
        fprintf(stderr, "Event: cannot register the event: %s\n", strerror(errno));
        return;
    }

    // Accept the client
    int client_ref = accept(io_watcher->fd, (struct sockaddr *) &client_addr, &socket_size);

    if(client_ref < 0){
        fprintf(stderr, "Accept: cannot accept the client: %s\n", strerror(errno));
        return;
    }

    if(client_ref > WORKING_CLIENT_COUNT){
        fprintf(stderr, "Accept: client_ref is too large, too many connections: %s\n", strerror(errno));
        return;
    }

    // Initialise the action (r/w) event, listen, then read/write to client
    ev_io_init(client_watcher, server_action_cb, client_ref, EV_READ);
    ev_io_start(loop, client_watcher);

    event_list[client_ref] = client_watcher;
}

void server_action_cb(struct ev_loop * loop, ev_io * io_watcher, int revents){

    // Declare client buffer and wipe it before use
    char * client_buffer = malloc(STRING_BUFFER_SIZE);
    memset(client_buffer, '\0', STRING_BUFFER_SIZE);

    // Detect if malloc failed
    if(!client_buffer){
        fprintf(stderr, "malloc: cannot allocate memory for client buffer: %s\n", strerror(errno));
        return;
    }

    // Detect if event register failed
    if (EV_ERROR & revents) {
        fprintf(stderr, "Event: cannot register the event: %s\n", strerror(errno));
        return;
    }

    // Here we go, start to read.
    ssize_t read_ref = recv(io_watcher->fd, client_buffer, STRING_BUFFER_SIZE, 0);

    if(read_ref < 0){
        fprintf(stderr, "Recv: data receive failed\n");
        free(client_buffer);
        return;
    } else if(read_ref == 0) {
        fprintf(stderr, "Recv: client disconnected.\n");
        free(client_buffer);
        server_event_cleanup(loop, io_watcher->fd);
        return;
    }

    // Print when data is received
    printf("Receive: got message from client with length %zd: %s", read_ref, client_buffer);

    // Send the echo back to client user
    ssize_t send_ref = send(io_watcher->fd, client_buffer, STRING_BUFFER_SIZE, 0);

    if(send_ref < 0){
        fprintf(stderr, "Send: data receive failed: %s\n", strerror(errno));
        free(client_buffer);
        return;
    } else if(send_ref == 0) {
        fprintf(stderr, "Send: client disconnected: %s\n", strerror(errno));
        free(client_buffer);
        server_event_cleanup(loop, io_watcher->fd);
        return;
    }

    // Copy the buffer to save_buffer and free up client_buffer
    strcpy(save_buffer, client_buffer);

    free(client_buffer);

}

void server_write_file_cb(struct ev_loop * loop, ev_periodic * timer_watcher, int revents){

    // Open file, with append mode
    FILE * file = fopen(SOCKS_LOG_FILE, "a+");

    if(!file){
        fprintf(stderr, "File: open failed: %s", strerror(errno));
    }

    // Write, flush, close
    fprintf(file, "%s", save_buffer);
    fflush(file);
    fclose(file);

    // Wipe the string buffer
    memset(save_buffer, '\0', STRING_BUFFER_SIZE);
}

void server_event_cleanup(struct ev_loop * loop, int ref){

    if(!event_list[ref]){
        printf("free: client has been freed.\n");
        return;
    }

    // Close this event first
    close(ref);
    ev_io_stop(loop, event_list[ref]);

    // Free up memory
    event_list[ref] = NULL;
    free(event_list[ref]);
}