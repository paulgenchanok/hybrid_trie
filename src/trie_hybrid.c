/** @file trie_hybrid.c
 * 
 * @brief A trie implementation ... but searching THROUGH CHILDREN IN A LIST.
 * 
 * @par
 * 
 */

#include <stdbool.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/trie_hybrid.h"


// Define and init globals.
//
rec_array_t * gp_recommends           = NULL;
char          g_word[MAX_WORD_LENGTH] = {0};
uint8_t       g_word_len              = 0;
void        * gp_curr                 = NULL;
bool          gb_found                = true;


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


htrie_t * htrie_create()
{

    htrie_t * p_new = calloc(1, sizeof(htrie_t));
    check_alloc(p_new, "htrie create", true);

    // p_root could be a void pointer and then have a type flag
    //
    p_new->p_root = htrie_create_dnode(); 
    gp_recommends = rec_array_create();

    gp_curr = p_new->p_root;

    return p_new;

}

void htrie_destroy(htrie_t * p_htrie)
{
    if (NULL == p_htrie)
    {
        return;
    }

    htrie_destroy_dnode(p_htrie->p_root, 1);
    p_htrie->p_root = NULL;

    free(p_htrie);
    p_htrie = NULL;

    rec_array_destroy(gp_recommends);
    gp_recommends = NULL;

}

static int8_t htrie_cnode_insert_worker(htrie_cnode_t * p_cnode, char * p_word)
{

    if ((NULL == p_cnode) || (NULL == p_word))
    {
        return -1;
    }

    // End condition. The node at p_node is the last one.
    //
    if ('\0' == *p_word)
    {
        if (POPULARITY_NOT_WORD == p_cnode->popularity)
        {
            p_cnode->popularity = POPULARITY_WORD;
            return 1;
        }

        else
        {
            p_cnode->popularity *= GAMMA;
            return 0;
        }
    }

    int8_t ret = 0;

    // It's not the end. Check if children is NULL to start.
    // Do an initial alloc if so.
    //
    if (NULL == p_cnode->p_children)
    {
        p_cnode->p_children = calloc(DEFAULT_CHARR_SIZE, \
            sizeof(htrie_cnode_t));
        check_alloc(p_cnode->p_children, "p_children alloc", true);

        // The array is created. Init the FIRST element to the character.
        //
        p_cnode->p_children[0].character = *p_word;

        // The parent node has one more child and a valid array now.
        //
        p_cnode->num_children++;
        p_cnode->array_size = DEFAULT_CHARR_SIZE;

        // Move on to the next character
        //
        p_word++;

        // Recurse
        //
        ret = htrie_cnode_insert_worker(&p_cnode->p_children[0], p_word);
    }

    else 
    {
        // There's already something there. Loop through and check.
        //
        bool b_found = false;

        for (uint8_t ind = 0; ind < p_cnode->num_children; ind++)
        {
            // If we found a child with the character we're looking for.
            //
            if (*p_word == p_cnode->p_children[ind].character)
            {
                // Recurse to follow on and then break. Set b_found.
                //
                b_found = true;
                p_word++;
                ret = htrie_cnode_insert_worker(&p_cnode->p_children[ind], \
                    p_word);
                break;
            }
        }

        // Okay. We didn't find the character. And it's not a null byte.
        // so we *have* to add it to p_children. We must.
        //
        if (false == b_found)
        {

            if (p_cnode->array_size == p_cnode->num_children)
            {
                // Realloc case
                //
                size_t new_size = (DEFAULT_CHARR_SIZE + p_cnode->array_size) \
                    * sizeof(htrie_cnode_t);

                p_cnode->p_children = realloc(p_cnode->p_children, new_size);

                check_alloc(p_cnode->p_children, "realloc", true);

                memset(p_cnode->p_children + p_cnode->array_size, '\0', \
                    DEFAULT_CHARR_SIZE * sizeof(htrie_cnode_t));

                p_cnode->array_size += DEFAULT_CHARR_SIZE;

            }

            // Realloc complete
            //
            uint8_t last_ele = p_cnode->num_children;

            // Set the new element
            //
            p_cnode->p_children[last_ele].character = *p_word;
            p_cnode->num_children++;

            // Next character and recurse
            //
            p_word++;

            ret = htrie_cnode_insert_worker(&p_cnode->p_children[last_ele], \
                p_word);

        }

    }

    return ret;


}


static int8_t htrie_insert(htrie_t * p_htrie, char * p_word)
{
    if ((NULL == p_htrie) || (NULL == p_htrie->p_root) || (NULL == p_word))
    {
        return -1;
    }

    // Okay. What a weird time this is. Gonna be super duper strange.
    // First. I guess. run through all the dnodes.
    //
    htrie_dnode_t * p_curr_dnode = p_htrie->p_root;
    uint8_t         depth        = 1;

    while (('\0' != *p_word) && (DEFAULT_HTRIE_DEPTH > depth))
    {

        uint8_t offset = *p_word - ASCII_PRINT_START;

        if (NULL == p_curr_dnode->p_children[offset])
        {
            // It's going to be a dnode. I THINK this will work.
            //
            htrie_dnode_t * p_new = htrie_create_dnode();
            p_new->character      = *p_word;
            p_new->depth          = depth;

            p_curr_dnode->p_children[offset] = p_new;

        }

        depth++;
        p_word++;
        p_curr_dnode = p_curr_dnode->p_children[offset];

    }

    // Most likely leave at depth == DEFAULT_HTRIE_DEPTH. But need to check
    //
    if ('\0' == *p_word)
    {

        if (POPULARITY_NOT_WORD == p_curr_dnode->popularity)
        {
            p_curr_dnode->popularity = POPULARITY_WORD;
        }

        else 
        {
            p_curr_dnode->popularity *= GAMMA;
        }
    }

    else 
    {
        // This means we left when DEFAULT_HTRIE_DEPTH == depth
        // Need to handle the transition case here. Correctly.
        //
        uint8_t offset = *p_word - ASCII_PRINT_START;

        if (NULL == p_curr_dnode->p_children[offset])
        {
            htrie_tnode_t * p_new = htrie_create_tnode();
            p_new->character      = *p_word;
            p_new->depth          = depth;

            p_curr_dnode->p_children[offset] = p_new;
        }

        p_word++;
        htrie_tnode_t * p_new_curr = p_curr_dnode->p_children[offset];

        // A microcosm check of the previous code
        if ('\0' == *p_word)
        {
            // It's a new word ending at the transition node.
            //
            p_new_curr->popularity = POPULARITY_WORD;
            // return 1 // new word
            return 1;
        }

        else 
        {
            // We have at least one more letter past. This means a cnode?
            // tnodes still have an array.
            uint8_t offset = *p_word - ASCII_PRINT_START;

            if (NULL == p_new_curr->p_children[offset])
            {
                htrie_cnode_t * p_cnew = htrie_create_cnode();
                p_cnew->character      = *p_word;

                p_new_curr->p_children[offset] = p_cnew;
            }

            // NOW I THINK we can run the insert worker.
            //
            p_word++;
            htrie_cnode_insert_worker(p_new_curr->p_children[offset], p_word);

        }
        
    }

    return 0;
}


static void htrie_get_cnode_words(htrie_cnode_t * p_cnode, \
                            char * prefix_buff,     \
                            uint8_t prefix_len,     \
                            uint8_t max_depth,      \
                            uint8_t level)
{

    if (level > max_depth)
    {
        return;
    }

    if (0 < level)
    {
        prefix_buff[prefix_len] = p_cnode->character;
        prefix_len++;
        prefix_buff[prefix_len] = '\0';
    }

    if (POPULARITY_NOT_WORD != p_cnode->popularity)
    {
        // printf("%s\n", prefix_buff);
        rec_array_insert(gp_recommends, prefix_buff, p_cnode->popularity);
    }

    for (uint8_t ind = 0; ind < p_cnode->num_children; ind++)
    {
        if (NULL != &p_cnode->p_children[ind])
        {
            htrie_get_cnode_words(&p_cnode->p_children[ind], \
                prefix_buff, \
                prefix_len, \
                max_depth,
                level + 1);
        }
    }


}


static void htrie_get_tnode_words(htrie_tnode_t * p_tnode, \
    char * p_prefix, uint8_t prefix_len, uint8_t max_depth)
{

        htrie_cnode_t * p_cnode = NULL;

        char prefix_buff[MAX_WORD_LENGTH] = {0};
        strncpy(prefix_buff, p_prefix, prefix_len);

        for (uint8_t ind = 0; ind < ALPHABET_SIZE; ind++)
        {

            p_cnode = p_tnode->p_children[ind];

            if (NULL != p_cnode)
            {
                // valid cnode child ...
                //
                prefix_buff[prefix_len] = p_cnode->character;
                htrie_get_cnode_words(p_cnode,  \
                                prefix_buff,    \
                                prefix_len,     \
                                max_depth,      \
                                1);


            }
        }

}


static void htrie_get_dnode_words(void * p_node,    \
            char prefix_buff[MAX_WORD_LENGTH],      \
            uint8_t prefix_len,                     \
            uint8_t depth,                          \
            uint8_t max_depth)
{

    if (DEFAULT_HTRIE_DEPTH == depth)
    {

        // We're actually at tnode.
        //
        htrie_tnode_t * p_tnode = (htrie_tnode_t *) p_node;

        prefix_buff[depth - 1] = p_tnode->character;
        prefix_buff[depth] = '\0';

        if (POPULARITY_NOT_WORD != p_tnode->popularity)
        {
            // printf("%s\n", prefix_buff);
            rec_array_insert(gp_recommends, prefix_buff, p_tnode->popularity);

        }

        htrie_get_tnode_words(p_tnode, prefix_buff, prefix_len, max_depth);
    }

    else 
    {

        htrie_dnode_t * p_dnode = (htrie_dnode_t *) p_node;


        for (uint8_t ind = 0; ind < ALPHABET_SIZE; ind++)
        {

            if (NULL != p_dnode->p_children[ind])
            {


                prefix_buff[depth - 1] = p_dnode->character;
                prefix_buff[depth] = '\0';

                if (POPULARITY_NOT_WORD != p_dnode->popularity)
                {
                    // printf("%s\n", prefix_buff);
                    rec_array_insert(gp_recommends, prefix_buff, \
                        p_dnode->popularity);

                }

                htrie_get_dnode_words(p_dnode->p_children[ind], prefix_buff, prefix_len + 1, depth + 1, max_depth - 1);
            }

        }
    }

}

// Only exists really for debugging purposes
//
static int8_t htrie_get_words(htrie_t * p_htrie, \
    char * p_prefix, uint8_t max_depth)
{

    if ((NULL == p_htrie) || (NULL == p_prefix))
    {
        return -1;
    }

    uint8_t         prefix_len    = strlen(p_prefix);
    htrie_dnode_t * p_curr        = p_htrie->p_root;
    char          * p_prefix_iter = p_prefix;
    uint8_t         depth         = 0;  // Starting at the root node.

    while (('\0' != *p_prefix_iter) && (DEFAULT_HTRIE_DEPTH > depth))
    {
        uint8_t offset = *p_prefix_iter - ASCII_PRINT_START;

        if (NULL != p_curr->p_children[offset])
        {
            p_curr = p_curr->p_children[offset];
            p_prefix_iter++;
            depth++;
        }

        else 
        {
            printf("DEBUG: Prefix or word not found!\n");
            return -1;
        }
    }

    htrie_cnode_t * p_cnode = NULL;

    if (DEFAULT_HTRIE_DEPTH == prefix_len)
    {
        htrie_get_tnode_words((htrie_tnode_t *) p_curr, \
            p_prefix, prefix_len, max_depth);

    }

    else if (DEFAULT_HTRIE_DEPTH > prefix_len)
    {

        // Woof case. Fairly complicated
        //
        char prefix_buff[MAX_WORD_LENGTH] = {0};
        strncpy(prefix_buff, p_prefix, prefix_len);

        htrie_get_dnode_words((htrie_dnode_t *) p_curr, \
            prefix_buff, prefix_len, depth, max_depth);

    }

    else 
    {

        // A little hairy but casts the void * p_curr to a tnode and then sees
        // if there's a cnode in its offset array
        //
        p_cnode = ((htrie_tnode_t *) p_curr)-> \
            p_children[*p_prefix_iter - ASCII_PRINT_START];

        if (NULL == p_cnode)
        {
            return -1;
        }

        // We've confirmed the next one is not NULL.
        // inc depth and the prefix iteration
        //
        depth++;
        p_prefix_iter++;

        // Go through the remainder of the prefix
        // end at the last node that matches the prefix
        //
        while (prefix_len > depth)
        {

            bool b_found = false;

            for (uint8_t ind = 0; ind < p_cnode->num_children; ind++)
            {

                if (*p_prefix_iter == p_cnode->p_children[ind].character)
                {
                    // it's found
                    p_cnode = &p_cnode->p_children[ind];
                    b_found = true;
                    break;
                }
            }

            if (false == b_found)
            {


                return -1;
            }

            depth++;
            p_prefix_iter++;
        }

        char prefix_buff[MAX_WORD_LENGTH] = {0};
        strncpy(prefix_buff, p_prefix, prefix_len);

        htrie_get_cnode_words(p_cnode, prefix_buff, prefix_len, max_depth, 0);
        
    }

    rec_array_print(gp_recommends);

    return 0;

}


// The problem is we don't know the current node TYPE.
// Do we add the word to the buffer before or after?
// BEFORE. 

static void * htrie_node_get_next(htrie_dnode_t * p_curr,   \
                    char      next,                         \
                    char    * p_prefix_buff,                \
                    uint8_t   prefix_len,                   \
                    uint8_t   max_depth)
{

    if ((NULL == p_curr) || (NULL == p_prefix_buff))
    {
        return NULL;
    }

    void * p_ret  = NULL;
    uint8_t depth = prefix_len;

    // Just check to see if it has it or no. Add it if so.

    if (DEFAULT_HTRIE_DEPTH > depth)
    {
        // p_curr is a dnode
        //
        p_ret = p_curr->p_children[next - ASCII_PRINT_START];

        if (NULL != p_ret)
        {
            p_prefix_buff[prefix_len] = next;
            p_prefix_buff[prefix_len + 1] = '\0';
        }

        return p_ret;

    }

    else if (DEFAULT_HTRIE_DEPTH == depth)
    {
        htrie_tnode_t * p_tnode = (htrie_tnode_t *) p_curr;

        p_ret = p_tnode->p_children[next - ASCII_PRINT_START];

        if (NULL != p_ret)
        {
            p_prefix_buff[prefix_len] = next;
            p_prefix_buff[prefix_len + 1] = '\0';
        }

        return p_ret;
    }

    else 
    {
        htrie_cnode_t * p_cnode = (htrie_cnode_t *) p_curr;

        for (uint8_t ind = 0; ind < p_cnode->num_children; ind++)
        {
            if (next == p_cnode->p_children[ind].character)
            {

                p_ret = &p_cnode->p_children[ind];
                p_prefix_buff[prefix_len] = next;
                p_prefix_buff[prefix_len + 1] = '\0';
                break;
            }
        }

        return p_ret;
    }
}



static rec_array_t * htrie_fill_recs(void * p_curr, 
                            char * p_prefix, 
                            uint8_t prefix_len, 
                            uint8_t max_depth)
{

    if ((NULL == p_curr) || (NULL == p_prefix))
    {
        return NULL;
    }

    uint8_t depth = prefix_len;

    if (DEFAULT_HTRIE_DEPTH == depth)
    {
        htrie_get_tnode_words((htrie_tnode_t *) p_curr, \
            p_prefix, prefix_len, max_depth);
    }

    else if (DEFAULT_HTRIE_DEPTH > depth)
    {
        char prefix_buff[MAX_WORD_LENGTH] = {0};
        strncpy(prefix_buff, p_prefix, prefix_len);

        htrie_get_dnode_words((htrie_dnode_t *) p_curr, \
            prefix_buff, depth, depth, max_depth);
    }

    else 
    {
        char prefix_buff[MAX_WORD_LENGTH] = {0};
        strncpy(prefix_buff, p_prefix, prefix_len);

        htrie_get_cnode_words((htrie_cnode_t *) p_curr, \
            prefix_buff, depth, max_depth, 0);
    }

    return gp_recommends;

}



// Loads a trie from a newline separated wordlist
//
int8_t htrie_load_wordlist(htrie_t * p_htrie, char * p_wordlist_path)
{
    if ((NULL == p_htrie) || (NULL == p_wordlist_path))
    {
        return -1;
    }

    FILE * p_wordlist = fopen(p_wordlist_path, "r");
    if (NULL == p_wordlist)
    {
        return -1;
    }

    char line_buff[MAX_WORD_LENGTH] = {0}; 
    // I'm gonna roll my own version of fgets

    uint8_t len = 0;
    char letter = '\0';

    while(true)
    {
        letter = fgetc(p_wordlist);

        if (EOF == letter)
        {
            break;
        }

        else if (('\n' == letter) || (' ' == letter))
        {
            line_buff[len] = '\0';
            htrie_insert(p_htrie, line_buff);
            len = 0;
        }

        else
        {
            //TODO: Cover case where the character is out of bounds too
            line_buff[len] = letter;
            len++;
        }

    }

    fclose(p_wordlist);
    p_wordlist = NULL;

    return 0;

}

static void htrie_update_node_popularity(void * p_node, uint8_t depth)
{
    if (NULL == p_node)
    {
        return;
    }

    float * p_old_popularity = NULL;

    if (DEFAULT_HTRIE_DEPTH > depth)
    {
        p_old_popularity = &((htrie_dnode_t *) p_node)->popularity;
    }

    else if (DEFAULT_HTRIE_DEPTH == depth)
    {
        p_old_popularity = &((htrie_tnode_t *) p_node)->popularity;
    }

    else 
    {
        p_old_popularity = &((htrie_cnode_t *) p_node)->popularity;
    }

    if (NULL != p_old_popularity)
    {
        if (POPULARITY_NOT_WORD == *p_old_popularity)
        {
            *p_old_popularity = POPULARITY_WORD;
        }

        else 
        {
            *p_old_popularity *= GAMMA;
        }
    }
}


int8_t htrie_get_recs(rec_array_t * p_dst)
{
    if (NULL == p_dst)
    {
        return -1;
    }

    // WARNING: This ONLY works because rec_array_t has is of a static size.
    // it has ZERO pointers. If rec_array_t is changed to have pointers
    // then such a shallow memcpy will not work.
    //
    memcpy(p_dst, gp_recommends, sizeof(rec_array_t));

    return 0;

}


int8_t htrie_put_letter(htrie_t * p_htrie, uint8_t letter)
{
    // TODO: Some input checking for letter here.
    //
    if (NULL == p_htrie)
    {
        return PUT_ERROR; // TODO: constants
    }

    uint8_t ret = PUT_CONT_FOUND; // Default.

    if ((' ' == letter) || ('\n' == letter))
    {
        g_word[g_word_len] = '\0';

        if (false == gb_found)
        {
            htrie_insert(p_htrie, g_word);
            ret = PUT_EOW_INSERT;
        }

        else 
        {
            htrie_update_node_popularity(gp_curr, g_word_len);
            ret = PUT_EOW_UPDATE;
        }

        // Reset globals for next word
        //
        g_word_len = 0;
        gb_found   = true;
        gp_curr    = p_htrie->p_root;

        return ret;

    }

    //TODO: Simplify this
    //
    if (true == gb_found)
    {
        gp_curr = htrie_node_get_next(gp_curr,  \
                                    letter,     \
                                    g_word,     \
                                    g_word_len, \
                                    DEFAULT_MAX_DEPTH);

        g_word[g_word_len] = letter;
        g_word_len++;

        if (NULL == gp_curr)
        {
            gb_found = false;
            ret      = PUT_CONT_NF_INIT;
        }

        else 
        {
            rec_array_clear(gp_recommends);
            htrie_fill_recs(gp_curr, g_word, g_word_len, DEFAULT_MAX_DEPTH);
            ret = PUT_CONT_FOUND;
        }

    }

    else 
    {
        // It's NOT Found.
        //
        g_word[g_word_len] = letter;
        g_word_len++;
        ret = PUT_CONT_NF_CONT;

    }

    return ret;

}


