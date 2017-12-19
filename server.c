//
// Created by hu on 12/19/17.
//

#include "server.h"

/**
 * Initialise the server mode
 *
 * Procedure:
 * socket ==> bind ==> listen ==> accept (event) ==> read (event) ==> write (event) => close
 */
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
        fprintf(stderr, "Socket: register failed, check your permission first.");
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
        fprintf(stderr, "Bind: bind failed, try again later.");
        exit(EXIT_FAILURE);
    }

    // Here we go: start to listen
    if(listen(socket_ref, SOMAXCONN) != 0){
        fprintf(stderr, "Listen: listen failed, try again later.");
        exit(EXIT_FAILURE);
    }

    // Reuse socket reference
    if(setsockopt(socket_ref, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        fprintf(stderr, "setsockopt: Reuse socket (SO_REUSEADDR) failed!");
        exit(EXIT_FAILURE);
    }

#ifdef SO_REUSEPORT
    if(setsockopt(socket_ref, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0){
        fprintf(stderr, "setsockopt: Reuse socket (SO_REUSEPORT) failed!");
        exit(EXIT_FAILURE);
    }
#endif

    // Register events
    ev_io_init(socket_watcher, server_accept_cb, socket_ref, EV_READ);
    ev_io_start(loop, socket_watcher);

    // Endlessly gets user query
    while(true)
        ev_run(loop, 0);

}

void server_accept_cb(struct ev_loop * loop, ev_io * io_watcher, int revents){



}

void server_read_cb(struct ev_loop * loop, ev_io * io_watcher, int revents){

}
