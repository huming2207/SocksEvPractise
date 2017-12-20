//
// Created by hu on 12/20/17.
//

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "data_list.h"

/**
 * Enqueue data to the queue, i.e. add new node to the last_node.
 * Also it needs to assign to first node if it's not initialised.
 *
 * @param data - String buffer need to be added
 * @return True if enqueued
 */
bool data_list_enqueue(char *data)
{
    // New node to add
    data_node * new_node;
    new_node = NULL;

    // Check the data first, don't let junk in
    if(!data){
        fprintf(stderr, "data_list_enqueue: failed to add data as it is not correctly initialised.");
        return false;
    }

    // Initialise the new node
    new_node = malloc(sizeof(data_node));

    // Check the malloc result
    if(!new_node) {
        fprintf(stderr, "data_list_enqueue: failed to initialise the node: %s", strerror(errno));
        return false;
    }

    // Initialise the string buffer inside the node
    new_node->data = malloc(strlen(data) + EXTRA_STRING_SPACES);

    // Check the data malloc result
    if(!new_node->data) {
        fprintf(stderr, "data_list_enqueue: failed to initialise the node: %s", strerror(errno));
        return false;
    }

    // Wipe the data and copy
    strcpy(new_node->data, data);

    // Initialise the first node if it's null
    if(!first_node) {

        // Put the new node to the first node and last node
        first_node = new_node;
        last_node = first_node;

        // Initialise the counter
        data_node_count = 0;

    } else {

        // Link the last_node to new node
        last_node->next_node = new_node;
        last_node = new_node;

        // Increase 1 to the counter
        data_node_count += 1;
    }

    return true;

}

/**
 * Dequeue one item from the queue, i.e. grab the first_node and delete it, then put the second one to the first_node
 * @return string in the first item
 */
char * data_list_dequeue()
{
    char * data = malloc(strlen(first_node->data) + EXTRA_STRING_SPACES);
    data_node * second_node;

    if(!data) {
        fprintf(stderr, "data_list_enqueue: failed to initialise the node: %s", strerror(errno));
        return false;
    }

    // Grab the first node's data to the data buffer
    strcpy(data, first_node->data);

    // Mark the first node's next node (second node)
    second_node = first_node->next_node;

    if(data_node_count < 1) {

        // Free up the first node and the last node
        free(first_node);
        free(last_node);
    } else {

        // Free the original first node and put the second node to the first node
        free(first_node);
        first_node = second_node;
    }

    // Set the counter
    data_node_count -= 1;


    return data;
}

char * get_first_node()
{
    return first_node->data;
}

char * get_last_node()
{
    return last_node->data;
}