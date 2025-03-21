/**
 * @file tkl_mutex.c
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
#include "tkl_mutex.h"
#include "tuya_error_code.h"
// --- END: user defines and implements ---

#ifdef TUYAOS_SUPPORT
/**
* @brief Create mutex
*
* @param[out] pMutexHandle: mutex handle
*
* @note This API is used to create and init a recursive mutex.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_mutex_create_init(TKL_MUTEX_HANDLE *pMutexHandle)
{
    // --- BEGIN: user implements ---
    if (NULL == pMutexHandle)
        return OPRT_INVALID_PARM;

    sys_mutex_init((os_mutex_t *)pMutexHandle);

    if (NULL == *pMutexHandle) {
        return OPRT_OS_ADAPTER_MUTEX_CREAT_FAILED;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
* @brief Lock mutex
*
* @param[in] mutexHandle: mutex handle
*
* @note This API is used to lock a recursive mutex.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_mutex_lock(CONST TKL_MUTEX_HANDLE mutexHandle)
{
    // --- BEGIN: user implements ---
    if (NULL == mutexHandle)
        return OPRT_INVALID_PARM;

    sys_mutex_get((os_mutex_t *)&mutexHandle);

    return OPRT_OK;
    // --- END: user implements ---
}

/**
* @brief Try Lock mutex
*
* @param[in] mutexHandle: mutex handle
*
* @note This API is used to try lock a recursive mutex.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_mutex_trylock(CONST TKL_MUTEX_HANDLE mutexHandle)
{
    // --- BEGIN: user implements ---
    INT32_T ret;
    if(!mutexHandle) {
        return OPRT_INVALID_PARM;
    }

    ret = sys_mutex_try_get((os_mutex_t *)&mutexHandle, 0);

    if (OS_OK != ret) {
        return OPRT_OS_ADAPTER_MUTEX_LOCK_FAILED;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
* @brief Unlock mutex
*
* @param[in] mutexHandle: mutex handle
*
* @note This API is used to unlock a recursive mutex.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_mutex_unlock(CONST TKL_MUTEX_HANDLE mutexHandle)
{
    // --- BEGIN: user implements ---
    if (NULL == mutexHandle)
        return OPRT_INVALID_PARM;

    sys_mutex_put((os_mutex_t *)&mutexHandle);

    return OPRT_OK;
    // --- END: user implements ---
}

/**
* @brief Release mutex
*
* @param[in] mutexHandle: mutex handle
*
* @note This API is used to release a recursive mutex.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tkl_mutex_release(CONST TKL_MUTEX_HANDLE mutexHandle)
{
    // --- BEGIN: user implements ---
    if (NULL == mutexHandle)
        return OPRT_INVALID_PARM;

    sys_mutex_free((os_mutex_t *)&mutexHandle);

    return OPRT_OK;
    // --- END: user implements ---
}

#endif /* TUYAOS_SUPPORT */
