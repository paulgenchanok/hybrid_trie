/** @file main.c
 * 
 * @brief
 * 
 * @par
 * 
 */

#include "../include/trie_hybrid.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>






int main(int argc, char ** argv)
{

    htrie_t * p_htrie = htrie_create();

    htrie_load_wordlist(p_htrie, "words_alpha.txt");

    // htrie_autotyper(p_htrie);

    htrie_destroy(p_htrie);

}
