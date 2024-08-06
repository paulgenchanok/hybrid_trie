/** @file main.c
 * 
 * @brief
 * 
 * @par
 * 
 */

#include "../include/trie_hybrid.h"
#include "../include/ipc.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


void shm_copy_recs(rec_array_t * p_recs, word_shm_t words[])
{

    for (uint8_t ind = 0; ind < p_recs->num_words; ind++)
    {
        words[ind].word_len = p_recs->word_array[ind].word_len;
        memcpy(words[ind].word,          \
            p_recs->word_array[ind].word, \
            words[ind].word_len);

        words[ind].word[words[ind].word_len] = '\0';

        printf("%s\n", words[ind].word);

    }

}

// void shm_serialize_recs(rec_array_t * p_recs, shm_data_t * p_data)
// {

//     p_data->num_recs = p_recs->num_words;

//     // uint8_t p_wdata = p_data->p_recs;
//     uint16_t offset = 0;

//     for (uint8_t ind = 0; ind < p_recs->num_words; ind++)
//     {

//         word_t curr = p_recs->word_array[ind];

//         memcpy(p_data->p_recs[offset], &curr.word_len, sizeof(uint8_t));
//         offset++;



//     }


// }


int main(int argc, char ** argv)
{

    htrie_t * p_htrie = htrie_create();

    htrie_load_wordlist(p_htrie, "words_alpha.txt");

    // htrie_autotyper(p_htrie);

    size_t segize = sizeof(shm_data_t);

    void * p_shm_addr = shm_user_create(SHM_NAME, segize);
    shm_data_init(p_shm_addr, segize);

    shm_data_t * p_data  = (shm_data_t *) p_shm_addr;

    rec_array_t * p_recs = NULL;

    uint16_t     len     = 0;
    uint8_t      letter  = '\0';
    bool         b_found = true;
    void       * p_curr  = p_htrie->p_root;

    // ONLY the Autocomplete daemon updates the prefix length.
    //

    while (true)
    {
        // The character is being sent.
        //
        sem_wait(&(p_data->sem_full));  // Init to 0. Immeidately block. 1->0

        len    = p_data->prefix_len;
        letter = p_data->prefix[len];

        printf("letter: %c. len %u\n", letter, len);

        if ((' ' == letter) || ('\n' == letter))
        {
            p_data->prefix[len] = '\0';

            if (false == b_found)
            {
                htrie_insert(p_htrie, p_data->prefix);
            }

            else 
            {
                htrie_update_node_popularity(p_curr, p_data->prefix_len);
            }

            p_data->prefix_len = 0;
            b_found            = true;
            p_curr             = p_htrie->p_root;
            sem_post(&(p_data->sem_empty)); // We've read it so it's "empty"
            continue;
        }

        if (true == b_found)
        {
            
            p_curr = htrie_node_get_next(p_curr,            \
                                        letter,             \
                                        p_data->prefix,     \
                                        p_data->prefix_len, \
                                        DEFAULT_MAX_DEPTH);

            if (NULL == p_curr)
            {
                

                b_found = false;
                p_data->prefix[len] = letter;
            }

            else 
            {
                p_data->prefix[len] = letter;
                p_recs = htrie_fill_recs(p_curr, \
                    p_data->prefix, len + 1, DEFAULT_MAX_DEPTH);

                shm_copy_recs(p_recs, p_data->rec_words);
                
            }

            p_data->prefix_len++;

        }

        else 
        {
            p_data->prefix[len] = letter;
            p_data->prefix_len++;
        }

        sem_post(&(p_data->sem_empty)); // We've read it so it's "empty"

    }

    // OK. Take user input here?


    shm_user_destroy(SHM_NAME);

    htrie_destroy(p_htrie);

}
