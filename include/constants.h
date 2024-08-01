/** @file constants.h
 * 
 * @brief
 * 
 * @par
 * 
 */

#pragma once

// These ASCII ranges only really affect dnodes
// cnodes can in theory handle any uint8_t value
//
#define EXTENDED_ASCII

#ifdef EXTENDED_ASCII

#define ASCII_PRINT_START   33      // !
#define ASCII_PRINT_END     126     // ~s
#define ALPHABET_SIZE       93      // 126 - 33

#else

#define ASCII_PRINT_START   97      // a
#define ASCII_PRINT_END     122     // z
#define ALPHABET_SIZE       26      //

#endif

#define GAMMA               1.01    // Learning factor

#define POPULARITY_WORD     0.5     // float
#define POPULARITY_NOT_WORD 0       // float

#define MAX_WORD_LENGTH     64

#define NUM_RECOMMENDS      3   // How many num recs to store

#define DEFAULT_MAX_DEPTH   5   // How far to go from prefix to fetch recs
#define DEFAULT_HTRIE_DEPTH 3   // How many "hash table" like levels to have
#define DEFAULT_CHARR_SIZE  1   // The initial size of the array


