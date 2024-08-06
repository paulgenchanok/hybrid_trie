/** @file ipc.h
 * 
 * @brief Implement an IPC mechanism for recommendations
 * 
 * @par
 * 
 */

#pragma once

#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "constants.h"

#define SHM_ERROR -1

#define MODE_NOT_FOUND  -1
#define MODE_IN_PROGRESS 0

typedef struct _word_shm {

    uint8_t word_len;
    char    word[MAX_WORD_LENGTH];

} word_shm_t;


typedef struct _shm_data
{

    sem_t       sem_full;
    sem_t       sem_empty;
    size_t      segsize;
    int8_t      mode;
    char        prefix[MAX_WORD_LENGTH];
    uint16_t    prefix_len;
    uint8_t     num_recs;
    // uint8_t   * p_recs[NUM_RECOMMENDS * (1 + MAX_WORD_LENGTH)];
    word_shm_t  rec_words[NUM_RECOMMENDS];

} shm_data_t;


/**
 * @brief Creates a shared memory segment with RW permissions.
 * Returns an error if the named shm already exists
 * 
 * @param p_shm_name The name of the shared memory segment
 * @param segment_size The segment size
 * @return A pointer to the shared memory segment. NULL on error
 */
void * shm_user_create(char * p_shm_name, size_t segment_size);


/**
 * @brief Opens a given shared memory segment with RW permissions
 * If the shm is not found, or permissions are lacking, returns an error
 * @param p_shm_name The shm segment to open
 * @return A pointer to the shm region. NULL on error.
 */
void * shm_user_open_rw(char * p_shm_name);

/**
 * @brief Destroys a shared memory region. Effectively a
 * wrapper around shm_unlink with perror printouts
 * 
 * @param p_name The shm to destroy
 */
void shm_user_destroy(const char * p_name);

void * shm_data_init(shm_data_t * p_shm_data, size_t segsize);

