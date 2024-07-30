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

    // trie_t * p_trie = create_trie();

    // trie_load_wordlist(p_trie, "corncob_30000");
    // trie_stat_print(p_trie);

    // destroy_trie(p_trie);
    // p_trie = NULL;

    // printf("Normal Trie Stuff Finished\n");

    // trie_csearch_t * p_ctrie = create_trie_csearch();

    // // printf("About to load a wordlist\n");

    // ctrie_load_wordlist(p_ctrie, "words_alpha.txt");
    // // ctrie_stat_walker(p_ctrie->p_root, p_ctrie);
    // // ctrie_stat_print(p_ctrie);
    // ctrie_level_children_print(p_ctrie->p_root, 0);

    // destroy_trie_csearch(p_ctrie);
    // p_ctrie = NULL;

    htrie_t * p_htrie = htrie_create();

    htrie_load_wordlist(p_htrie, "words_alpha.txt");

    htrie_get_words(p_htrie, "abac", 4);

    htrie_destroy(p_htrie);

    printf("destroyed?\n");

}
