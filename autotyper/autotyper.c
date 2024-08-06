/** @file autotyper.c
 * 
 * @brief Show's interaction with the autocomplete shared memory segment
 * hopefully useful in developing a decent API.
 * 
 * @par
 * 
 */

#include <ncurses.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include "../include/ipc.h"
#include "../include/constants.h"


// Now this will have to have Curses. The other will not.

int main(int argc, char ** argv)
{

    void * p_shm_addr = shm_user_open_rw(SHM_NAME);

    if (NULL == p_shm_addr)
    {
        return -1;
    }

    shm_data_t * p_data = (shm_data_t *) p_shm_addr;
    uint16_t len = p_data->prefix_len;

    char letter = '\0';

    initscr();

    while(true)
    {

        sem_wait(&(p_data->sem_empty));
        
        len = p_data->prefix_len;

        letter = getch();

        // put a character
        //
        p_data->prefix[len] = letter;

        sem_post(&(p_data->sem_full));

        printw("rec? : %s\n", p_data->rec_words[0].word);

    }

    endwin();

    return 0;

}