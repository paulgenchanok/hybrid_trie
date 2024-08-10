/** @file main.c
 * 
 * @brief
 * 
 * @par
 * 
 */

#include "../include/trie_hybrid.h"
#include <curses.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>


// Todo: clean these up.
//
static void htrie_clear_rec_line()
{
    int row = 0;
    int col = 0;

    getyx(stdscr, row, col);
    move(row + 1, 0);
    clrtoeol();
    move(row, col);

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

void htrie_render(rec_array_t * p_rat, htrie_render_action action)
{
    if (NULL == p_rat)
    {
        return;
    }

    switch (action) 
    {
        case PRINT:
            rec_array_print(p_rat);
        break;

        case CLEAR:
            rec_array_clear(p_rat);
            htrie_clear_rec_line();
        break;

        case CLEAR_LINE:
            htrie_clear_rec_line();
        break;

        default:
        break;

    }
}




int main(int argc, char ** argv)
{

    htrie_t * p_htrie = htrie_create();

    printf("Loading wordlist ...\n");

    htrie_load_wordlist(p_htrie, "words_alpha.txt");
    char        letter   = '\0';
    int8_t      retcode  = PUT_ERROR;
    rec_array_t rat      = {0};

    initscr();

    while (true)
    {

        letter  = getch();
        retcode = htrie_put_letter(p_htrie, letter);

        switch (retcode) 
        {

            case PUT_ERROR:
            break;

            case PUT_EOW_INSERT: // Fall through
            case PUT_EOW_UPDATE:
                htrie_render(&rat, CLEAR);
            break;

            case PUT_CONT_FOUND:

                htrie_get_recs(&rat);
                htrie_render(&rat, PRINT);
            break;

            case PUT_CONT_NF_INIT:
                htrie_render(&rat, CLEAR_LINE);
            break;

            case PUT_CONT_NF_CONT: // Do nothing.
            break;

            default:
            break;

        }

    }

    endwin();

    htrie_destroy(p_htrie);

}
