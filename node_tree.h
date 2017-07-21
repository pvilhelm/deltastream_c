#pragma once

#include <stdint.h>

#include "node.h"

#define NTREE_NO_ERROR 0
#define NTREE_NODE_EXISTS 1
#define NTREE_NODE_DOESNT_EXIST 2

/* Convenience macro to cast ip and port field of node_t to a uint64 key */
#define KEY(n) (*((uint64_t*)&((n)->ip)))

typedef struct tree_node tree_node_t;

typedef struct tree_node {
    tree_node_t* n_r;    /* Higher ip:port */
    tree_node_t* n_l;    /* Lower ip:port */
    node_t* n;      /* This node */
} tree_node_t;

tree_node_t* add_child(tree_node_t* root, node_t* n);
tree_node_t* remove_child(tree_node_t* root, node_t* n);
tree_node_t* find_child(tree_node_t* root, uint32_t ip, uint16_t port);

int free_all_children(tree_node_t* root);
int remove_and_free_child(tree_node_t* root, node_t* child);