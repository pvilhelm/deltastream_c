#include <stdlib.h>

#include "node_tree.h"

tree_node_t * add_child(tree_node_t * root, node_t * n)
{
    if(!root || !n) /* Invalid input */
        return 0;
    tree_node_t* c_n = root; /* Current_Node */
    while (c_n) {
        if (KEY(c_n->n) == KEY(n))
            return 0; /* Trying to add node with same key */
        else if (KEY(n) > KEY(c_n->n)) { /* Right child */
            if (c_n->n_r) {/* Right child exists*/
                c_n = c_n->n_r;
            } else { /* Else make a wrapper and add n to it */
                c_n->n_r = calloc(1,sizeof(tree_node_t));
                c_n->n_r->n = n;
                return c_n->n_r;
            }
        } else /* if (KEY(n) < KEY(c_n->n)) */ { /* Left child */
            if (c_n->n_l) { /* Left child exists */
                c_n = c_n->n_l;
            } else { /* Else make a wrapper and add n to it */
                c_n->n_l = calloc(1, sizeof(tree_node_t));
                c_n->n_l->n = n;
                return c_n->n_l;
            }
        } 

    }

    return 0;
}

tree_node_t * remove_child(tree_node_t * root, node_t * n)
{
    return 0;
}

tree_node_t * find_child(tree_node_t * root, uint32_t ip, uint16_t port)
{
    return 0;
}

int free_all_children(tree_node_t * root)
{
    return 0;
}

int remove_and_free_child(tree_node_t * root, node_t * child)
{
    return 0;
}
