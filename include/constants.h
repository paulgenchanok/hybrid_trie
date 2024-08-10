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

#ifdef ASCII_azAZ

#define ASCII_AZ_START 65   // A
#define ASCII_AZ_MIDZ  90   // Z
#define ASCII_AZ_MIDa  97   // a
#define ASCII_AZ_END   122  // z
#define ALPHABET_SIZE  52

#endif

#define GAMMA               1.01    // Learning factor

#define POPULARITY_WORD     0.5     // float
#define POPULARITY_NOT_WORD 0       // float

#define MAX_WORD_LENGTH     64

#define NUM_RECOMMENDS      3   // How many num recs to store

#define DEFAULT_MAX_DEPTH   5   // How far to go from prefix to fetch recs
#define DEFAULT_HTRIE_DEPTH 3   // How many "hash table" like levels to have


#define PUT_ERROR           -1
#define PUT_EOW_INSERT      0x00
#define PUT_EOW_UPDATE      0x01
#define PUT_CONT_FOUND      0x10
#define PUT_CONT_NF_INIT    0x11 // First time it's not found
#define PUT_CONT_NF_CONT    0x12 // All other times.

