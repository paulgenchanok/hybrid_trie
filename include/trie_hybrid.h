/** @file trie_hybrid.h
 * 
 * @brief A trie implementation ... but as a HYBRID
 * 
 * @par
 * 
 */

#pragma once

#include <unistd.h>
#include <stdint.h>

#include "rec_array.h"
#include "htrie_structs.h"
#include "constants.h"


typedef enum
{
    PRINT,
    CLEAR,
    CLEAR_LINE,

} htrie_render_action;

htrie_t * htrie_create();

int8_t htrie_load_wordlist(htrie_t * p_htrie, char * p_wordlist_path);

void htrie_destroy(htrie_t * p_htrie);

int8_t htrie_put_letter(htrie_t * p_htrie, uint8_t letter);

int8_t htrie_get_recs(rec_array_t * p_dst);
