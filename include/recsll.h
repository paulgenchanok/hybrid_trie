/** @file recsll.h
 * 
 * @brief A trie implementation ... but searching THROUGH CHILDREN IN A LIST.
 * 
 * @par
 * 
 */

 #pragma once

 #include <stdint.h>
 #include "constants.h"

 // YES IT'S A CIRCULARLY LINKED LIST. LET'S GO.
typedef  struct _rec_word {

    char               word[MAX_WORD_LENGTH];
    float              popularity;
    struct _rec_word * p_next;
    struct _rec_word * p_prev;

} rec_word_t;

typedef struct _recs_cll {

    rec_word_t * p_first;           // The first word
    uint8_t      num_words;         // Anything after this is stale
    float        popularity_min;    // For efficiency

} recs_cll_t;



rec_word_t * rec_word_create();

void rec_word_destroy(rec_word_t * p_recw);

recs_cll_t * recs_ll_create();

void recs_ll_dequeue(recs_cll_t * p_recsll);

void recs_ll_insert(recs_cll_t * p_recsll, char * p_word, float popularity);

void recs_ll_print(recs_cll_t * p_recsll);

void recs_ll_destroy(recs_cll_t * p_recsll);
