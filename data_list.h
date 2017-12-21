//
// Created by hu on 12/20/17.
//

#ifndef SOCKSEVPRACTISE_DATA_LIST_H
#define SOCKSEVPRACTISE_DATA_LIST_H

#include <stdlib.h>
#include <stdbool.h>

// Add 1 more spaces for '\0'.
#define EXTRA_STRING_SPACES     1

typedef struct _data_node
{
  char *data;
  struct _data_node *next_node;
} data_node;

static int data_node_count;

bool data_list_enqueue(char *data);

char *data_list_dequeue();

#endif //SOCKSEVPRACTISE_DATA_LIST_H
