/** @file recsll.c
 * 
 * @brief A trie implementation ... but searching THROUGH CHILDREN IN A LIST.
 * 
 * @par
 * 
 */

#include <curses.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include "../include/rec_array.h"

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

rec_array_t * rec_array_create()
{

    rec_array_t * p_new = calloc(1, sizeof(rec_array_t));
    check_alloc(p_new, "rec array create", true);

    return p_new;
}

void rec_array_destroy(rec_array_t * p_rat)
{

    if (NULL == p_rat)
    {
        return;
    }

    free(p_rat);
    p_rat = NULL;

}


void rec_array_clear(rec_array_t * p_rat)
{
    if (NULL == p_rat)
    {
        return;
    }

    // egads, memset.
    //
    memset(p_rat->word_array, 0, NUM_RECOMMENDS * sizeof(word_t));
    p_rat->num_words      = 0;
    p_rat->min_popularity = 0;
}


void rec_array_dequeue(rec_array_t * p_rat, uint8_t index)
{
    if (NULL == p_rat)
    {
        return;
    }

    uint8_t ind = NUM_RECOMMENDS - 1; // linked list it basically.

    for (; ind > index; ind--)
    {
        uint8_t prev_len = p_rat->word_array[ind-1].word_len;
        float   prev_pop = p_rat->word_array[ind-1].popularity;


        strncpy(p_rat->word_array[ind].word,    \
            p_rat->word_array[ind-1].word,      \
            prev_len);

        p_rat->word_array[ind].word[prev_len] = '\0';
        p_rat->word_array[ind].word_len = prev_len;
        p_rat->word_array[ind].popularity = prev_pop;
    }

    p_rat->min_popularity = p_rat->word_array[NUM_RECOMMENDS - 1].popularity;

}


void rec_array_insert(rec_array_t * p_rat, char * p_word, float popularity)
{

    if (p_rat->min_popularity >= popularity)
    {
        return;
    }

    uint8_t word_len = strlen(p_word);
    uint8_t ind      = 0;
    bool    b_hit    = false;

    word_t * p_curr = NULL; // maybe should be a pointer? I'll know when it segfaults.

    for (; ind < NUM_RECOMMENDS; ind++)
    {
        p_curr = &p_rat->word_array[ind];

        if (word_len == p_curr->word_len)
        {
            int cmp = strncmp(p_word, p_curr->word, word_len);

            if (0 == cmp)
            {
                // it's a duplicate. no action
                break;
            }
            
        }

        if (p_curr->popularity < popularity)
        {
            // we have our insert index
            b_hit = true;
            break;
        }
    }

    if (true == b_hit)
    {

        if (NUM_RECOMMENDS > p_rat->num_words)
        {
            p_rat->num_words++;
        }

        rec_array_dequeue(p_rat, ind);

        strncpy(p_curr->word, p_word, word_len);
        p_curr->word[word_len] = '\0';
        p_curr->popularity = popularity;
        p_curr->word_len = word_len;
    }    

}

void rec_array_print(rec_array_t * p_rat)
{
    if (NULL == p_rat)
    {
        return;
    }

    int row = 0;
    int col = 0;

    getyx(stdscr, row, col);
    move(row + 1, 0);
    clrtoeol();

    for (uint8_t ind = 0; ind < p_rat->num_words; ind++)
    {
        printw("%s ", p_rat->word_array[ind].word);
    }

    move(row, col);

}