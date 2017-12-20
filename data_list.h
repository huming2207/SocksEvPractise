//
// Created by hu on 12/20/17.
//

#ifndef SOCKSEVPRACTISE_DATA_LIST_H
#define SOCKSEVPRACTISE_DATA_LIST_H

#include <stdlib.h>
#include <stdbool.h>

// Add 3 more spaces for '\0', '\n' and '\r'.
#define EXTRA_STRING_SPACES     3

typedef struct _data_node
{
    char * data;
    struct _data_node * next_node;
} data_node;

data_node * first_node;
data_node * last_node;
int data_node_count;

bool data_list_enqueue(char *data);
char * data_list_dequeue();
char * get_first_node();
char * get_last_node();



#endif //SOCKSEVPRACTISE_DATA_LIST_H
