/**
 * @file tkl_memory.c
 * @brief this file was auto-generated by tuyaos v&v tools, developer can add implements between BEGIN and END
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 *           changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

// --- BEGIN: user defines and implements ---
#include "wrapper_os.h"
#include "tkl_memory.h"
#include "tuya_error_code.h"
// --- END: user defines and implements ---

#ifdef TUYAOS_SUPPORT
/**
* @brief Alloc memory of system
*
* @param[in] size: memory size
*
* @note This API is used to alloc memory of system.
*
* @return the memory address malloced
*/
VOID_T *tkl_system_malloc(SIZE_T size)
{
    // --- BEGIN: user implements ---
    if (size == 0)
        return NULL;

    return sys_malloc(size);
    // --- END: user implements ---
}

/**
* @brief Free memory of system
*
* @param[in] ptr: memory point
*
* @note This API is used to free memory of system.
*
* @return VOID_T
*/
VOID_T tkl_system_free(VOID_T* ptr)
{
    // --- BEGIN: user implements ---
    if (ptr == NULL)
        return;

    sys_mfree(ptr);
    return;
    // --- END: user implements ---
}

/**
 * @brief Allocate and clear the memory
 *
 * @param[in]       nitems      the numbers of memory block
 * @param[in]       size        the size of the memory block
 *
 * @return the memory address calloced
 */
VOID_T *tkl_system_calloc(size_t nitems, size_t size)
{
    // --- BEGIN: user implements ---
    if (size && nitems > (~(size_t) 0) / size)
        return NULL;
    return sys_calloc(nitems, size);
    // --- END: user implements ---
}

/**
 * @brief Re-allocate the memory
 *
 * @param[in]       nitems      source memory address
 * @param[in]       size        the size after re-allocate
 *
 * @return VOID_T
 */
VOID_T *tkl_system_realloc(VOID_T* ptr, size_t size)
{
    // --- BEGIN: user implements ---
    return sys_realloc(ptr, size);
    // --- END: user implements ---
}

/**
* @brief Get system free heap size
*
* @param none
*
* @return heap size
*/
INT_T tkl_system_get_free_heap_size(VOID_T)
{
    // --- BEGIN: user implements ---
    return sys_free_heap_size();
    // --- END: user implements ---
}

#endif /* TUYAOS_SUPPORT */
