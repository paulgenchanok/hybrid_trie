/** @file htrie_structs.c
 * 
 * @brief A trie implementation ... but searching THROUGH CHILDREN IN A LIST.
 * 
 * @par
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/htrie_structs.h"



/*!
 * @brief Checks if memory allocation is successful. This can be applied
    to other functions which return NULL when encountering an error
    @param[in] p_mem is a pointer to the memory block to check
    @param[in] p_msg is the error message header
    @param[in] b_hard_fail specifies whether to fail by returning p_mem
    or by exiting the program altogether. This precludes graceful exits.
    It is possible to add a setjmp argument and jump to an exit func.
 * @return Returns a created trie node, or NULL on error.
 */
static void * check_alloc(void * p_mem, char * p_msg, bool b_hard_fail)
{

    if ((NULL == p_mem) && (true == b_hard_fail))
    {
        perror(p_msg);
        exit(EXIT_FAILURE);
    }

    return p_mem;
}


htrie_dnode_t * htrie_create_dnode()
{
    htrie_dnode_t * p_new = calloc(1, sizeof(htrie_dnode_t));
    check_alloc(p_new, "htrie create dnode", true);

    for (uint8_t ind = 0; ind < ALPHABET_SIZE; ind++)
    {
        p_new->p_children[ind] = NULL;
    }

    return p_new;
}

void htrie_destroy_dnode(htrie_dnode_t * p_dnode, uint8_t depth)
{
    if (NULL == p_dnode)
    {
        return;
    }

    for (uint8_t ind = 0; ind < ALPHABET_SIZE; ind++)
    {
        // It's not that simple any more.
        //
        if (depth < DEFAULT_HTRIE_DEPTH)
        {
            // It's smaller. It's still a dnode.
            //
            htrie_destroy_dnode((htrie_dnode_t *) p_dnode->p_children[ind], \
                depth + 1);
        }

        else if (depth == DEFAULT_HTRIE_DEPTH)
        {
            // The pointer is actually is tnode. Recursive free should handle it
            // > inb4, segfault
            // > actually debugged a lot of segfaults here
            //
            htrie_destroy_tnode((htrie_tnode_t *) p_dnode->p_children[ind]);
        }

        // No particular need for an else case
        //

    }

    free(p_dnode);
    p_dnode = NULL;
}


htrie_cnode_t * htrie_create_cnode()
{
    htrie_cnode_t * p_new = calloc(1, sizeof(htrie_cnode_t));
    check_alloc(p_new, "htrie create cnode", true);

    p_new->p_children = NULL; // Is this necessary even?

    return p_new;

}

void htrie_destroy_cnode(htrie_cnode_t * p_cnode)
{
    if (NULL == p_cnode)
    {
        return;
    }

    for (uint8_t ind = 0; ind < p_cnode->array_size; ind++)
    {
        htrie_destroy_cnode(&(p_cnode->p_children[ind]));
    }

    free(p_cnode->p_children);
    p_cnode->p_children = NULL;

    //cnode is never actually freed. Needs to be this way for recursion.
    //
}

htrie_tnode_t * htrie_create_tnode()
{

    htrie_tnode_t * p_new = calloc(1, sizeof(htrie_tnode_t));
    check_alloc(p_new, "htrie create tnode", true);

    for (uint8_t ind = 0; ind < ALPHABET_SIZE; ind++)
    {
        p_new->p_children[ind] = NULL;
    }

    return p_new;

}

void htrie_destroy_tnode(htrie_tnode_t * p_tnode)
{

    if (NULL == p_tnode)
    {
        return;
    }

    for (uint8_t ind = 0; ind < ALPHABET_SIZE; ind++)
    {
        htrie_destroy_cnode(p_tnode->p_children[ind]);

        free(p_tnode->p_children[ind]);
        p_tnode->p_children[ind] = NULL;
    }

    free(p_tnode);
    p_tnode = NULL;
}
