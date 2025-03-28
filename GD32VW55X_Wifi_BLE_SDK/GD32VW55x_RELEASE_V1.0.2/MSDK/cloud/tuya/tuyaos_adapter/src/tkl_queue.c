/**
 * @file tkl_queue.c
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
#include "gd32vw55x.h"
#include "tkl_queue.h"
#include "tuya_error_code.h"
// --- END: user defines and implements ---

#ifdef TUYAOS_SUPPORT
/**
 * @brief Create message queue
 *
 * @param[in] msgsize message size
 * @param[in] msgcount message number
 * @param[out] queue the queue handle created
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_queue_create_init(TKL_QUEUE_HANDLE *queue, INT_T msgsize, INT_T msgcount)
{
    // --- BEGIN: user implements ---
    if (NULL == queue) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (!sys_queue_init(queue, msgcount, msgsize))
        return OPRT_OK;

    return OPRT_OS_ADAPTER_QUEUE_CREAT_FAILED;
    // --- END: user implements ---
}

/**
 * @brief post a message to the message queue
 *
 * @param[in] queue the handle of the queue
 * @param[in] data the data of the message
 * @param[in] timeout timeout time
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_queue_post(CONST TKL_QUEUE_HANDLE queue, VOID_T *data, UINT_T timeout)
{
    // --- BEGIN: user implements ---
    INT_T ret;

    if (NULL == queue) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (__get_CONTROL() == 1) {
        ret = sys_queue_write((os_queue_t *)&queue, data, 0, true);
    } else {
        ret = sys_queue_post_with_timeout((os_queue_t *)&queue, data, timeout);
    }

    if (ret) {
        return OPRT_OS_ADAPTER_QUEUE_SEND_FAIL;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief fetch message from the message queue
 *
 * @param[in] queue the message queue handle
 * @param[inout] msg the message fetch form the message queue
 * @param[in] timeout timeout time
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_queue_fetch(CONST TKL_QUEUE_HANDLE queue, VOID_T *msg, UINT_T timeout)
{
    // --- BEGIN: user implements ---
    VOID_T *dummyptr;
    INT_T ret;

    if (NULL == queue) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (msg == NULL) {
        msg = &dummyptr;
    }

    if (timeout == TKL_QUEUE_WAIT_FROEVER) {
        ret = sys_queue_fetch((os_queue_t *)&queue, msg, 0, TRUE);
    } else {
        ret = sys_queue_fetch((os_queue_t *)&queue, msg, timeout, (timeout == 0) ? FALSE : TRUE);
    }

    if (ret) {
        return OPRT_OS_ADAPTER_QUEUE_SEND_FAIL;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief free the message queue
 *
 * @param[in] queue the message queue handle
 *
 * @return VOID_T
 */
VOID_T tkl_queue_free(CONST TKL_QUEUE_HANDLE queue)
{
    // --- BEGIN: user implements ---
    if (NULL == queue) {
        return;
    }

    if (sys_queue_cnt((os_queue_t *)&queue)) {
        /* Line for breakpoint.  Should never break here! */
        __NOP();
    }

    sys_queue_free((os_queue_t *)&queue);
    return;
    // --- END: user implements ---
}

#endif /* TUYAOS_SUPPORT */
