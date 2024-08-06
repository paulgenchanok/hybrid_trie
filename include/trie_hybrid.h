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

int8_t htrie_get_words(htrie_t * p_htrie, char * p_prefix, uint8_t max_depth);

void htrie_render(rec_array_t * p_rat, htrie_render_action action);

void htrie_autotyper(htrie_t * p_htrie);
