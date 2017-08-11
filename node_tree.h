#pragma once

#include <stdint.h>

#include "node.h"

#define NTREE_NO_ERROR 0
#define NTREE_NODE_EXISTS 1
#define NTREE_NODE_DOESNT_EXIST 2


struct tree_node;

struct tree_node {
    struct tree_node* n_p;    /* Parent node */
    struct tree_node* n_r;    /* Higher ip:port */
    struct tree_node* n_l;    /* Lower ip:port */
    node_t* n;      /* This node */
};

struct tree_node *add_child(struct tree_node* root, node_t* n);
struct tree_node *remove_child(struct tree_node* n);
struct tree_node *find_child(struct tree_node* root, uint32_t ip, uint16_t port);

int free_all_children(struct tree_node* root, int free_nodes);
int remove_and_free_child(struct tree_node* root, node_t* child);
uint64_t key(node_t* n);