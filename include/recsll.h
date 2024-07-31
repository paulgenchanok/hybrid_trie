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
 // how to avoid adding duplicates?
 //
typedef  struct _rec_word {

    char               word[MAX_WORD_LENGTH];
    uint8_t            word_len;
    float              popularity;
    struct _rec_word * p_next;
    struct _rec_word * p_prev;

} rec_word_t;

typedef struct _recs_cll {

    rec_word_t * p_first;           // The first word
    uint8_t      num_words;         // Anything after this is stale
    float        popularity_min;    // For efficiency

} recs_cll_t;

// Implementing in a ciruclarly linked list is a NIGHTMARE
// I think I can do it JUST AS WELL with an array. Geez.
// Need to clear in between each character being typed.
//

typedef struct _word {

    uint8_t word_len;
    char    word[MAX_WORD_LENGTH];
    float   popularity;

} word_t;

typedef struct _rec_array {

    word_t  word_array[NUM_RECOMMENDS];
    uint8_t num_words;
    float   min_popularity;

} rec_array_t;


rec_word_t * rec_word_create();

void rec_word_destroy(rec_word_t * p_recw);

recs_cll_t * recs_ll_create();

void recs_ll_dequeue(recs_cll_t * p_recsll);

void recs_ll_insert(recs_cll_t * p_recsll, char * p_word, float popularity);

void recs_ll_print(recs_cll_t * p_recsll);

void recs_ll_clear(recs_cll_t * p_recsll);

void recs_ll_destroy(recs_cll_t * p_recsll);

rec_array_t * rec_array_create();

void rec_array_destroy(rec_array_t * p_rat);

void rec_array_clear(rec_array_t * p_rat);

void rec_array_dequeue(rec_array_t * p_rat, uint8_t index);

void rec_array_insert(rec_array_t * p_rat, char * p_word, float popularity);

void rec_array_print(rec_array_t * p_rat);
