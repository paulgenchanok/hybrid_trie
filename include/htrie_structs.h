/** @file htrie_structs.h
 * 
 * @brief A trie implementation ... but as a HYBRID
 * 
 * @par
 * 
 */

#pragma once

#include <stdint.h>
#include "constants.h"



typedef struct _htrie_depth_node {

    void    * p_children[ALPHABET_SIZE];    // 8
    uint8_t   depth;                        // 1 
    uint8_t   character;                    // 1
    float     popularity;                   // 2

} htrie_dnode_t;

typedef struct _htrie_cnode {

    struct _htrie_cnode * p_children;       // 8
    uint8_t               num_children;     // 1
    uint8_t               array_size;       // 1
    uint8_t               character;        // 1
    float                 popularity;       // 2

} htrie_cnode_t;


typedef struct _htrie_transition_node {

    htrie_cnode_t * p_children[ALPHABET_SIZE];  // 8
    uint8_t         depth;                      // 1
    uint8_t         character;                  // 1
    float           popularity;                 // 2


} htrie_tnode_t;

typedef struct _htrie {

    htrie_dnode_t * p_root; 

} htrie_t;


htrie_dnode_t * htrie_create_dnode();

void htrie_destroy_dnode(htrie_dnode_t * p_dnode, uint8_t depth);

htrie_cnode_t * htrie_create_cnode();

void htrie_destroy_cnode(htrie_cnode_t * p_cnode);

htrie_tnode_t * htrie_create_tnode();

void htrie_destroy_tnode(htrie_tnode_t * p_tnode);