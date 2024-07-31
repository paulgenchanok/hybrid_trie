/** @file recsll.c
 * 
 * @brief A trie implementation ... but searching THROUGH CHILDREN IN A LIST.
 * 
 * @par
 * 
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "../include/recsll.h"

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



rec_word_t * rec_word_create()
{
    rec_word_t * p_new = calloc(1, sizeof(rec_word_t));
    check_alloc(p_new, "rec word alloc", true);

    return p_new;
}

void rec_word_destroy(rec_word_t * p_recw)
{
    if (NULL == p_recw)
    {
        return;
    }

    free(p_recw);
    p_recw = NULL;

}

recs_cll_t * recs_ll_create()
{
    recs_cll_t * p_new = calloc(1, sizeof(recs_cll_t));
    check_alloc(p_new, "recs ll alloc", true);

    p_new->p_first = rec_word_create();
    rec_word_t * p_curr = p_new->p_first;

    for (uint8_t ind = 0; ind < NUM_RECOMMENDS - 1; ind++)
    {

        if (NULL == p_curr->p_next)
        {
            p_curr->p_next = rec_word_create();
            p_curr->p_next->p_prev = p_curr;
            p_curr = p_curr->p_next;
        }

    }

    p_curr->p_next = p_new->p_first;
    p_new->p_first->p_prev = p_curr;

    return p_new;
}

void recs_ll_dequeue(recs_cll_t * p_recsll)
{
    if ((NULL == p_recsll) || (NULL == p_recsll->p_first))
    {
        return;
    }

    // Dequeue the last and update recsll
    //
    rec_word_t * p_last = p_recsll->p_first->p_prev;

    if (NULL == p_last)
    {
        return; // I don't think this case will ever be met?
    }

    rec_word_t * p_new_last = p_last->p_prev;

    rec_word_destroy(p_last);
    p_last = NULL;

    p_recsll->p_first->p_prev = p_new_last;
    p_recsll->popularity_min  = p_new_last->popularity;

}

void recs_ll_insert(recs_cll_t * p_recsll, char * p_word, float popularity)
{
    if ((NULL == p_recsll) || (NULL == p_word))
    {
        return;
    }

    if ((p_recsll->popularity_min >= popularity) && \
        (NUM_RECOMMENDS == p_recsll->num_words))
    {
        return; // The word is too unpopular. Don't humor it.
    }

    // We know we have to insert a new word SOMEWHERE now.
    //
    rec_word_t * p_curr   = p_recsll->p_first;
    uint8_t      word_len = strlen(p_word);
    bool         b_hit    = false;
    bool         b_dup    = false;
    uint8_t      ind      = 0;

    for (ind = 0; ind < p_recsll->num_words; ind++)
    {

        if (word_len == p_curr->word_len)
        {

            int cmp = strncmp(p_curr->word, p_word, word_len);

            if (cmp == 0)
            {
                b_dup = true;
                break;
            }

        }

        if (p_curr->popularity < popularity)
        {
            b_hit = true;
            break;
        }

        p_curr = p_curr->p_next;
    }

    if (true == b_hit)
    {

        if (NUM_RECOMMENDS - 1 == ind)
        {
            // The very last element
            //
            strncpy(p_curr->word, p_word, word_len);
            p_curr->word[word_len]   = '\0';
            p_curr->word_len         = word_len;
            p_curr->popularity       = popularity;
            p_recsll->popularity_min = popularity;
        }

        else 
        {
        
            recs_ll_dequeue(p_recsll);

            rec_word_t * p_prior = p_curr->p_prev;
            rec_word_t * p_new   = rec_word_create();
            p_new->popularity    = popularity;
            p_new->word_len      = word_len;
            strncpy(p_new->word, p_word, word_len);

            // Already null terminated.
            p_prior->p_next = p_new;
            p_new->p_prev   = p_prior;
                
            p_new->p_next   = p_curr;
            p_curr->p_prev  = p_new;

            if (0 == ind)
            {
                p_recsll->p_first = p_new;
            }
        
        }

    }

    if ((NUM_RECOMMENDS > p_recsll->num_words) && (false == b_dup))
    {

        strncpy(p_curr->word, p_word, word_len);
        p_curr->word[word_len] = '\0';
        p_curr->word_len       = word_len;
        p_curr->popularity     = popularity;
        p_recsll->num_words++;

    }

}

void recs_ll_print(recs_cll_t * p_recsll)
{
    if ((NULL == p_recsll) || (NULL == p_recsll->p_first))
    {
        return;
    }

    uint8_t      ind    = 0;
    rec_word_t * p_curr = p_recsll->p_first;

    int row = 0;
    int col = 0;

    getyx(stdscr, row, col);
    move(row + 1, 0);
    clrtoeol();

    for (uint8_t ind = 0; ind < p_recsll->num_words; ind++)
    {
        printw("%s ", p_curr->word);
        p_curr = p_curr->p_next;
    }

    move(row, col);

}

void recs_ll_clear(recs_cll_t * p_recsll)
{
    if (NULL == p_recsll)
    {
        return;
    }

    p_recsll->num_words = 0;
    p_recsll->popularity_min = 0;
}

void recs_ll_destroy(recs_cll_t * p_recsll)
{

    if ((NULL == p_recsll) || (NULL == p_recsll->p_first))
    {
        return;
    }

    rec_word_t * p_curr = p_recsll->p_first;
    p_recsll->p_first   = NULL;

    for (uint8_t ind = 0; ind < p_recsll->num_words; ind++)
    {
        rec_word_t * p_next = p_curr->p_next;
        rec_word_destroy(p_curr);
        p_curr = p_next;
    }

    free(p_recsll);
    p_recsll = NULL;

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

// static void rec_array_insert_len(rec_array_t * p_rat, char * p_word)
// {

//     uint8_t word_len = strlen(p_word);

//     for (uint8_t ind = 0; ind < NUM_RECOMMENDS; ind++)
//     {

//     }

// }



void rec_array_insert(rec_array_t * p_rat, char * p_word, float popularity)
{




}