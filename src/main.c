/** @file main.c
 * 
 * @brief
 * 
 * @par
 * 
 */

#include "../include/trie_hybrid.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv)
{

    htrie_t * p_htrie = htrie_create();

    htrie_load_wordlist(p_htrie, "words_alpha.txt");

    // htrie_get_words(p_htrie, "abac", 4);

    htrie_autotyper(p_htrie);

    htrie_destroy(p_htrie);

}
