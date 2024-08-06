/** @file ipc.c
 * 
 * @brief Implement an IPC mechanism for recommendations
 * 
 * @par
 * 
 */

#include <string.h>
#include "../include/ipc.h"


/**
 * @brief Creates a shared memory segment with RW permissions.
 * Returns an error if the named shm already exists
 * 
 * @param p_shm_name The name of the shared memory segment
 * @param segment_size The segment size
 * @return A pointer to the shared memory segment. NULL on error
 */
void * shm_user_create(char * p_shm_name, size_t segment_size)
{

    if (NULL == p_shm_name)
    {
        return NULL;
    }

    // Only create the shm region with read-write perms and
    // opened in read-write mode.
    // Returns error if it already exists
    //
    int    flags = O_CREAT | O_EXCL | O_RDWR;
    mode_t perms = S_IRUSR | S_IWUSR; // Read, write.

    int shm_fd = shm_open(p_shm_name, flags, perms);

    if (SHM_ERROR == shm_fd)
    {
        perror("shm create: shm_open");
        return NULL;
    }

    int ftrunc_ret = ftruncate(shm_fd, segment_size);

    if (-1 == ftrunc_ret)
    {
        perror("shm create: ftruncate");
        return NULL;
    }

    // Map the shared memory object. Lets' go.
    //
    void * p_shm_addr = \
    mmap(NULL, segment_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memset(p_shm_addr, 0, segment_size);

    if(MAP_FAILED == p_shm_addr)
    {
        perror("mmap");
        return NULL;
    }

    int cret = close(shm_fd); // Don't need it anymore

    if (-1 == cret)
    {
        perror("shm create: close");
        shm_user_destroy(p_shm_name);
        p_shm_addr = NULL;
        return NULL;
    }

    return p_shm_addr;

}

/**
 * @brief Opens a given shared memory segment with RW permissions
 * If the shm is not found, or permissions are lacking, returns an error
 * @param p_shm_name The shm segment to open
 * @return A pointer to the shm region. NULL on error.
 */
void * shm_user_open_rw(char * p_shm_name)
{

    if (NULL == p_shm_name)
    {
        return NULL;
    }

    int shm_fd = shm_open(p_shm_name, O_RDWR, 0);

    if (SHM_ERROR == shm_fd)
    {
        perror("shm_user_open open");
        return NULL;
    }

    struct stat sbuff;
    fstat(shm_fd, &sbuff);

    void * p_shm_addr = \
    mmap(NULL, sbuff.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (MAP_FAILED == p_shm_addr)
    {
        perror("mmap");
        return NULL;
    }

    int cret = close(shm_fd);

    if (-1 == cret)
    {
        perror("shm user open rw close");
    }

    return p_shm_addr;    

}

/**
 * @brief Destroys a shared memory region. Effectively a
 * wrapper around shm_unlink with perror printouts
 * 
 * @param p_name The shm to destroy
 */
void shm_user_destroy(const char * p_name)
{
    if (NULL == p_name)
    {
        return;
    }

    int unlink_ret = shm_unlink(p_name);

    if (-1 == unlink_ret)
    {
        perror("shm_unlink");
    }

}

/**
 * @brief Initializes a block of shared memory with the shm metadata
 * structure that is meant to be accessed by the proxy and cache
 * 
 * @param p_shm_mdata A pointer to the metadata structure in shared memory
 * @param segsize The size of the segment
 * @return NULL on error. p_shm_mdata on success
 */
void * shm_data_init(shm_data_t * p_shm_data, size_t segsize)
{
    if (NULL == p_shm_data)
    {
        return NULL;
    }

    if (sizeof(shm_data_t) > segsize)
    {
        return NULL;
    }

    sem_init(&(p_shm_data->sem_full),  1, 0);
    sem_init(&(p_shm_data->sem_empty), 1, SEM_INIT_SEMAPHORE);
    memset(p_shm_data->rec_words, 0, NUM_RECOMMENDS*sizeof(word_shm_t));

    p_shm_data->segsize    = segsize;
    p_shm_data->mode       = 0;
    p_shm_data->prefix_len = 0;

    return p_shm_data;

}

