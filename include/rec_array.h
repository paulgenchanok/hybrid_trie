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


rec_array_t * rec_array_create();

void rec_array_destroy(rec_array_t * p_rat);

void rec_array_clear(rec_array_t * p_rat);

void rec_array_dequeue(rec_array_t * p_rat, uint8_t index);

void rec_array_insert(rec_array_t * p_rat, char * p_word, float popularity);

void rec_array_print(rec_array_t * p_rat);

void rec_array_copy_word(word_t * p_dst, word_t * p_src);
