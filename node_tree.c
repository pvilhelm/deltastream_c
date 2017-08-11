#include <stdlib.h>

#include "node_tree.h"
#include <string.h>

struct tree_node *add_child(struct tree_node *root, node_t *n)
{
    if(!root || !n) /* Invalid input */
        return 0;

    struct tree_node* c_n = root; /* Current_Node */

    if(!c_n->n){
        c_n->n = n;
        return c_n;
    }

    while (c_n) {
        if (key(c_n->n) == key(n))
            return 0; /* Trying to add node with same key */
        else if (key(n) > key(c_n->n)) { /* Right child */
            if (c_n->n_r) {/* Right child exists*/
                c_n = c_n->n_r;
            } else { /* Else make a wrapper and add n to it */
                c_n->n_r = calloc(1,sizeof(struct tree_node));
                c_n->n_r->n = n;
                c_n->n_r->n_p = c_n;
                return c_n->n_r;
            }
        } else /* if (key(n) < key(c_n->n)) */ { /* Left child */
            if (c_n->n_l) { /* Left child exists */
                c_n = c_n->n_l;
            } else { /* Else make a wrapper and add n to it */
                c_n->n_l = calloc(1, sizeof(struct tree_node));
                c_n->n_l->n = n;
                c_n->n_l->n_p = c_n;
                return c_n->n_l;
            }
        }
    }
    return 0;
}

uint64_t key(node_t *n) {
    return ( (uint64_t) n->ip << 16) + n->port;
}

struct tree_node *remove_child(struct tree_node *n)
{
    struct tree_node* c_n = n;

    if(!n)
        return 0;

    /* If to be removed node has no children */
    if (!c_n->n_l && !c_n->n_r) {
        /* If current node has parent */
        if (c_n->n_p) {
            /* Set the correct ptr in parent to 0 */
            if (c_n->n_p->n_l == c_n)
                c_n->n_p->n_l = 0;
            else
                c_n->n_p->n_r = 0;
        }
        return c_n;
    }

    /* If the current node has only a right child */
    if (!c_n->n_l && c_n->n_r) {
        if(c_n->n_p){ /* If current node is not root */
            if (c_n->n_p->n_l == c_n){
                c_n->n_p->n_l = c_n->n_r;
            } else {
                c_n->n_p->n_r = c_n->n_r;
            }
            /* Set child parent to current node's parent */
            c_n->n_r->n_p = c_n->n_p;
            return c_n;
        } else { /* Current node is root */
            struct tree_node* ret_n = malloc(sizeof(struct tree_node));
            memcpy(ret_n, c_n, sizeof(struct tree_node));
            ret_n->n_r = c_n;
            c_n->n_l = c_n->n_r->n_l;
            c_n->n_r = c_n->n_r->n_r;
            c_n->n = c_n->n_r->n;
            free(c_n->n_r);
            return ret_n;
        }
    }
    else if(c_n->n_l && !c_n->n_r) {
        if (c_n->n_p) { /* If current node is not root */
            if (c_n->n_p->n_l == c_n) {
                c_n->n_p->n_l = c_n->n_l;
            } else {
                c_n->n_p->n_r = c_n->n_l;
            }
            /* Set child parent to current node's parent */
            c_n->n_l->n_p = c_n->n_p;
            return c_n;
        } else { /* Current node is root */
            struct tree_node* ret_n = malloc(sizeof (struct tree_node));
            memcpy(ret_n,c_n,sizeof(struct tree_node));
            ret_n->n_l = c_n;
            c_n->n_l = c_n->n_l->n_l;
            c_n->n_r = c_n->n_l->n_r;
            c_n->n = c_n->n_l->n;
            free(c_n->n_l);
            return ret_n;
        }
    } else { /* Node has two children */
        /* Find leftest node on the right */
        struct tree_node* n_min = c_n->n_r;
        while (1) {
            if(n_min->n_l)
                n_min = n_min->n_l;
            else
                break;
        }
        remove_child(n_min);

        /* If current node is not root */
        if(c_n->n_p){
            n_min->n_l = c_n->n_l;
            n_min->n_r = c_n->n_r;
            n_min->n_p = c_n->n_p;
            /* Set child parent to current node's parent */
            if(c_n->n_p->n_l == c_n)
                c_n->n_p->n_l = n_min;
            else
                c_n->n_p->n_r = n_min;

            /* Set parent of children of current node to leftest node */
            return c_n;
        } else {
            struct tree_node* ret_n = malloc(sizeof(struct tree_node));
            memcpy(ret_n, c_n, sizeof(struct tree_node));
            c_n->n = n_min->n;
            return ret_n;          
        }
    }
}

struct tree_node *find_child(struct tree_node *root, uint32_t ip, uint16_t port)
{
    /* Make a target key from ip and port */
    uint64_t k = (((uint64_t) ip << 16) + port);

    struct tree_node* c_n = root; /* Current_Node */
    while (c_n) {
        if (key(c_n->n) == k)
            return c_n; /* This node is the node */
        else if (k > key(c_n->n)) { /* Right child */
            c_n = c_n->n_r;
        } else /* if (key(n) < key(c_n->n)) */ { /* Left child */
            c_n = c_n->n_l;
        }
    }
    return 0; /* No such child exists */
}

int free_all_children(struct tree_node *root, int free_nodes)
{
    int freed_children = 0;
    struct tree_node *c_n = root; 
    while (c_n) {
        if(c_n->n_r){
            c_n = c_n->n_r;
            continue;
        } else if (c_n->n_l) {
            c_n = c_n->n_l;
            continue; 
        } else {
            if(c_n->n_p){
                if(c_n->n_p->n_l == c_n)
                    c_n->n_p->n_l = 0;
                else
                    c_n->n_p->n_r = 0;
                if(free_nodes)
                    free(c_n->n);
                struct tree_node* p_n = c_n->n_p;
                free(c_n);
                freed_children++;
                c_n = p_n;
                continue;
            } else {
                freed_children++;
                if (free_nodes)
                    free(c_n->n);
                free(c_n);
                c_n = 0; 
            }
        }

    }
    return freed_children;
}

int remove_and_free_child(struct tree_node *root, node_t *child)
{
    struct tree_node *rem = find_child(root, child->ip, child->port);
    if (rem) {
        remove_child(rem);
        free(rem);
        return 1;
    } else
        return 0;
}
