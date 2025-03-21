/*!
    \file    wrapper_os.h
    \brief   Header file for OS wrapper.

    \version 2023-07-20, V1.0.0, firmware for GD32VW55x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef __WRAPPER_OS_H
#define __WRAPPER_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "wrapper_os_config.h"

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
#ifndef EXTERN
#define EXTERN extern
#endif
#undef EXTERN
/*============================ LOCAL VARIABLES ===============================*/
/*============================ MACROS ========================================*/
#define OS_OK            0
#ifndef PLATFORM_OS_LITEOS
#define OS_ERROR        -1
#endif
#define OS_TIMEOUT      -2

/* CPU sleep mode */
#define SYS_PS_OFF                    0
#define SYS_PS_DEEP_SLEEP             1

#define TASK_PRIO_HIGHER(n)           (n)
#define TASK_PRIO_LOWER(n)            (-n)

/* RTOS uses an internal variable to record the nesting, so we no longer need a variable here */
#define SYS_SR_ALLOC()

/* OS IRQ service hook called just after the ISR starts */
#define sys_int_enter()
/* OS IRQ service hook called before the ISR exits */
#define sys_int_exit()

typedef void *os_sema_t;
typedef void *os_mutex_t;
typedef void *os_task_t;
typedef void *os_timer_t;
typedef void *os_queue_t;
typedef unsigned long os_prio_t;

typedef void (*task_func_t)(void *argv);
typedef void (*timer_func_t)(void *p_tmr, void *p_arg);

/*============================ MACRO FUNCTIONS ===============================*/
#define sys_zalloc(a)                  sys_calloc(a, 1)

#define SYS_TIME_AFTER(a, b)           ((int32_t)(b) - (int32_t)(a) < 0)
#define SYS_TIME_BEFORE(a, b)          SYS_TIME_AFTER(b, a)
#define SYS_TIME_AFTER_EQ(a, b)        ((int32_t)(a) - (int32_t)(b) >= 0)
#define SYS_TIME_BEFORE_EQ(a, b)       SYS_TIME_AFTER_EQ(b, a)

#define sys_task_create_dynamic(name, stack_size, priority, func, ctx)  sys_task_create(NULL, name, NULL, stack_size, 0, 0, priority, func, ctx)

/*============================ PROTOTYPES ====================================*/
/*!
    \brief      allocate a block of memory with a minimum of 'size' bytes.
    \param[in]  size: the minimum size of the requested block in bytes
    \param[out] none
    \retval     address to allocated memory, NULL pointer if there is an error
*/
void *sys_malloc(size_t size);

/*!
    \brief      allocate a certian chunks of memory with specified size
                Note: The allocated memory is filled with bytes of value zero.
                All chunks in the allocated memory are contiguous.
    \param[in]  count: multiple number of size want to malloc
    \param[in]  size:  number of size want to malloc
    \param[out] none
    \retval     address to allocated memory, NULL pointer if there is an error
*/
void *sys_calloc(size_t count, size_t size);

/*!
    \brief      free a memory to the heap
    \param[in]  ptr: pointer to the address want to free
    \param[out] none
    \retval     none
*/
void sys_mfree(void *ptr);

/*!
    \brief      change the size of a previously allocated memory block.
    \param[in]  mem: address to the old buffer
    \param[in]  size: number of the new buffer size
    \param[out] none
    \retval     address to allocated memory, NULL pointer if there is an error
*/
void *sys_realloc(void *mem, size_t size);

/*!
    \brief      get system free heap size
    \param[in]  none
    \param[out] none
    \retval     system free heap size value(0x00000000-0xffffffff)
*/
int32_t sys_free_heap_size(void);

/*!
    \brief      get minimum free heap size that has been reached
    \param[in]  none
    \param[out] none
    \retval     system minimum free heap size value(0x00000000-0xffffffff)
*/
int32_t sys_min_free_heap_size(void);

/*!
    \brief      get system least heap block size
                Note: it's used to count all memory used by heap and heap management structures.
                    A least block is usually consist of a heap block header and a minimum block.
                    However heap allocation may not align the allocated heap area with this minimum block,
                    the counting results may be inaccurate.
    \param[in]  none
    \param[out] none
    \retval     system heap block size value in bytes(0x0000-0xffff)
*/
uint16_t sys_heap_block_size(void);

/*!
    \brief      get heap info
    \param[in]  total_size: get total heap size
    \param[in]  free_size: get free heap size
    \param[in]  min_free_size: get minimum free heap size
    \param[out] none
    \retval     none
*/
void sys_heap_info(int *total_size, int *free_size, int *min_free_size);

/*!
    \brief      set the content of the buffer to specified value
    \param[in]  s: The address of a buffer
    \param[in]  c: the value want to memset
    \param[in]  count: count value want to memset
    \param[out] none
    \retval     none
*/
void sys_memset(void *s, uint8_t c, uint32_t count);

/*!
    \brief      copy buffer content from source address to destination address.
    \param[in]  src: the address of source buffer
    \param[in]  n: the length to copy
    \param[out] des: the address of destination buffer
    \retval     none
*/
void sys_memcpy(void *des, const void *src, uint32_t n);

/*!
    \brief      move buffer content from source address to destination address
                Note: It could work between two overlapped buffers.
    \param[in]  src: the address of source buffer
    \param[in]  n: the length to move
    \param[out] des: the address of destination buffer
    \retval     none
*/
void sys_memmove(void *des, const void *src, uint32_t n);

/*!
    \brief      compare two buffers
    \param[in]  buf1: address to the source buffer 1
    \param[in]  buf2: address to the source buffer 2
    \param[in]  count: the compared buffer size in bytes
    \param[out] none
    \retval      0 if buf1 equals buf2, non-zero otherwise.
*/
int32_t sys_memcmp(const void *buf1, const void *buf2, uint32_t count);

/*!
    \brief      create a task wrapping a task handle and a message queue
                Note:  Message queue wrapped in the task_wrapper_t is created to only hold pointers,
                  therefore the objects pointed by these pointers are not thread safe. You need to protect
                  them for some scenarios.
    \param[in]  static_tcb: pointer to the task buffer
    \param[in]  name: the task's name
    \param[in]  stack_base: the task's stack base address
    \param[in]  stack_size: the task's stack size, in words (4 bytes)
    \param[in]  queue_size: the task's message queue size
    \param[in]  queue_item_size: the task's message queue item size
    \param[in]  priority: the task's priority
    \param[in]  func: the task's entry function
    \param[in]  ctx: the task's parameter
    \param[out] none
    \retval     the task handle if succeeded, NULL otherwise.
*/
void *sys_task_create(void *static_tcb, const uint8_t *name, uint32_t *stack_base, uint32_t stack_size,
                    uint32_t queue_size, uint32_t queue_item_size, uint32_t priority, task_func_t func, void *ctx);

/*!
    \brief      return RTOS task name
    \param[in]  task: pointer to the task handle
    \param[out] none
    \retval     task name
*/
char* sys_task_name_get(void *task);

/*!
    \brief      delete a task and free all associated resources
                Note: Message queue wrapped in the task_wrapper_t is created to only hold pointers,
                  therefore the objects pointed by these pointers are not thread safe. You need to protect
                  them for some scenarios.
    \param[in]  task: pointer to the task handle, delete the current task if it's NULL
    \param[out] none
    \retval     none
*/
void sys_task_delete(void *task);

/*!
    \brief      list statistics for all tasks
    \param[in]  none
    \param[out] pwrite_buf: pointer to the result buffer, expect a statistics string
    \retval     none
*/
void sys_task_list(char *pwrite_buf);

/*!
    \brief      wait for a message from the message queue wrapped in the current task
    \param[in]  timeout_ms: millisecond timeout
                            0 means forever,
                            if the time is less than a tick, will roll up to 1 tick
    \param[out] msg_ptr: pointer to the message
    \retval     0 if succeeded, OS_ERROR otherwise
 */
int32_t sys_task_wait(uint32_t timeout_ms, void *msg_ptr);

/*!
    \brief      post a message to the message queue wrapped in the target task
    \param[in]  receiver_task: pointer to the message receiver task handle
    \param[in]  msg_ptr: pointer to the message
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                    with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     function run status,
                0 if succeeded, OS_ERROR otherwise.
 */
int32_t sys_task_post(void *receiver_task, void *msg_ptr, uint8_t from_isr);

/*!
    \brief      flush all messages of the message queue wrapped in the target task
                Note: All messages will be recycled to the free queue and the task will be resumed to
                  run after the operation, since we are queuing pointers in the task wrapped message queue,
                  do not to forget to handle all these pointers well to avoid memory leak.
    \param[in]  task: pointer to the task handle
    \param[out] none
    \retval     none
*/
void sys_task_msg_flush(void *task);

/*!
    \brief      get the number of waiting messages in the task queue
    \param[in]  task: pointer to the task handle
    \param[in]  from_isr: whether is from ISR
    \param[out] none
    \retval     the number of waiting messages if succeeded, OS_ERROR otherwise.
 */
int32_t sys_task_msg_num(void *task, uint8_t from_isr);

/*!
    \brief      return RTOS current task handle
    \param[in]  none
    \param[out] none
    \retval     current task handle
*/
os_task_t sys_current_task_handle_get(void);

/*!
    \brief     get the handle of the OS idle task
    \param[in]  none
    \param[out] none
    \retval     the pointer to task handle
*/
os_task_t *sys_idle_task_handle_get(void);

/*!
    \brief      return RTOS current task stack depth from special sp index
    \param[in]  cur_sp sp index
    \param[out] none
    \retval     stack depth
*/
int32_t sys_current_task_stack_depth(unsigned long cur_sp);

/*!
    \brief      get the handle of the OS timer task
    \param[in]  none
    \param[out] none
    \retval     the pointer to task handle
*/
os_task_t *sys_timer_task_handle_get(void);

/*!
    \brief      get the free stack size of the target task
    \param[in]  task: the pointer to the task handle
    \param[out] none
    \retval     the free stack size in words (4 bytes)(0x00000000-0xffffffff)
*/
uint32_t sys_stack_free_get(void *task);

/*!
    \brief      initialize notification for a WIFI task.
    \param[in]  task: pointer to the task handle
    \param[out] none
    \retval     return 0 on success and != 0 if error occurred.
*/
int sys_task_init_notification(void *task);

/*!
    \brief      task suspend itself until it is notified (or timeout expires)
    \param[in]  timeout: Maximum duration to wait, in ms, if no notification is pending.
 *                       0 means do not wait and -1 means wait indefinitely.
    \param[out] none
    \retval     the number of pending notification (0 if timeout was reached)
*/
int sys_task_wait_notification(int timeout);

/*!
    \brief      send notification to a task
    \param[in]  task: Task to notify.
    \param[in]  isr: Indicate if this is called from ISR.
    \param[out] none
    \retval     none
*/
void sys_task_notify(void *task, bool isr);

/*!
    \brief      create and initialize a semaphore extend
    \param[in]  sema: the pointer to the semaphore handle
    \param[in]  max_count: max count of semaphore
    \param[in]  init_count: initialize count of semaphore
    \param[out] sema: pointer to the semaphore
    \retval     OS_OK if succeeded, OS_ERROR otherwise.
*/
int32_t sys_sema_init_ext(os_sema_t *sema, int max_count, int init_count);

/*!
    \brief      create and initialize a semaphore
    \param[in]  sema: the pointer to the semaphore handle
    \param[in]  init_val: initialize vlaue of semaphore
    \param[out] sema: pointer to the semaphore
    \retval     OS_OK if succeeded, OS_ERROR otherwise.
*/
int32_t sys_sema_init(os_sema_t *sema, int32_t init_val);

/*!
    \brief      free a semaphore
    \param[in]  sema: pointer to the semaphore handle
    \param[out] none
    \retval     none
*/
void sys_sema_free(os_sema_t *sema);

/*!
    \brief      release a semaphore
                Note:  It could be called either in task context or interrupt context except for some OS
                  that has seperated implementations for the different context, like FreeRTOS
    \param[in]  sema: pointer to the semaphore handle
    \param[out] none
    \retval     none
*/
void sys_sema_up(os_sema_t *sema);

/*!
    \brief      release a semaphore in a interrupt context, it's only meaningful to some OS
                with implementations seperated for different execution context, like FreeRTOS
    \param[in]  sema: pointer to the semaphore handle
    \param[out] none
    \retval     none
*/
void sys_sema_up_from_isr(os_sema_t *sema);

//void sys_post_sema(os_sema_t *sema);

/*!
    \brief      require the semaphore within a given time constraint
    \param[in]  sema: pointer to the semaphore handle
    \param[in]  timeout_ms: millisecond value of timeout,
                    0 means forever,
                    if the time is less than a tick, will roll up to 1 tick.
    \param[out] none
    \retval     function run status
      \arg        OS_TIMEOUT: timeout
      \arg        OS_OK: run success
*/
int32_t sys_sema_down(os_sema_t *sema, uint32_t timeout_ms);

/*!
    \brief      return a semaphore count.
    \param[in]  sema: pointer to the semaphore handle
    \param[out] none
    \retval     semaphore count.
*/
int sys_sema_get_count(os_sema_t *sema);

/*!
    \brief      create and initialize a mutex
    \param[in]  mutex: pointer to the mutext handle
    \param[out] none
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int sys_mutex_init(os_mutex_t *mutex);

/*!
    \brief      free a mutex
    \param[in]  mutex: pointer to the mutex handle
    \param[out] none
    \retval     none
*/
void sys_mutex_free(os_mutex_t *mutex);

/*!
    \brief      try to require the mutex
    \param[in]  mutex: pointer to the mutex handle
    \param[in]  timeout: Maximum duration to wait, in ms.
                         0 means do not wait and -1 means wait indefinitely.
    \param[out] none
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int32_t sys_mutex_try_get(os_mutex_t *mutex, int timeout);

/*!
    \brief      require the mutex
    \param[in]  mutex: pointer to the mutex handle
    \param[out] none
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int32_t sys_mutex_get(os_mutex_t *mutex);

/*!
    \brief      release a mutex
                Note: It could be only called in task context for some OS
                  that has seperated implementations for the different context, like FreeRTOS
    \param[in]  mutex: pointer to the mutext
    \param[out] none
    \retval     none
*/
void sys_mutex_put(os_mutex_t *mutex);

/*!
    \brief      create and initialize a message queue
    \param[in]  queue_size: queue size
    \param[in]  item_size: queue item size
    \param[out] queue: pointer to the queue
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int32_t sys_queue_init(os_queue_t *queue, int32_t queue_size, uint32_t item_size);

/*!
    \brief      free a message queue
    \param[in]  queue: pointer to the queue
    \param[out] none
    \retval     none
*/
void sys_queue_free(os_queue_t *queue);

/*!
    \brief      post a message to the target message queue
    \param[in]  queue: pointer to the queue handle
    \param[in]  msg: pointer to the message
    \param[out] none
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int32_t sys_queue_post(os_queue_t *queue, void *msg);

/*!
    \brief      post a message to the target message queue
    \param[in]  msg: pointer to the message
    \param[in]  timeout_ms: queue post timeout time in ms
    \param[in]  pointer to the queue handle
    \param[out] none
    \retval     function run status
      \arg        OS_ERROR: return error
      \arg        OS_OK: run success
*/
int32_t sys_queue_post_with_timeout(os_queue_t *queue, void *msg, int32_t timeout_ms);

/*!
    \brief      fetch a message from the message queue within a given time constraint
    \param[in]  queue: pointer to the queue handle
    \param[in]  timeout_ms: timeout in ms. For blocking operation, 0 means forever, if the time is
                    less than a tick, will roll up to 1 tick; for non-blocking operation, it's unused.
    \param[in]  is_blocking: whether it's a blocking operation
    \param[out] msg: pointer to the message
    \retval     function run status
      \arg        OS_TIMEOUT: timeout
      \arg        OS_OK: run success
*/
int32_t sys_queue_fetch(os_queue_t *queue, void *msg, uint32_t timeout_ms, uint8_t is_blocking);

/*!
    \brief      check if a RTOS message queue is empty or not.
    \param[in]  queue: pointer to the queue handle
    \param[out] none
    \retval     true if queue is empty, false otherwise.
*/
bool sys_queue_is_empty(os_queue_t *queue);

/*!
    \brief      get the number of messages pending a queue.
    \param[in]  queue: pointer to the queue handle
    \param[out] none
    \retval     The number of messages pending in the queue.
*/
int sys_queue_cnt(os_queue_t *queue);

/*!
    \brief      write a message at the end of a RTOS message queue.
    \param[in]  queue: pointer to the queue handle
    \param[in]  msg: Message to copy in the queue. (It is assume that buffer is of the size specified in @ref os_queue_t_create)
    \param[in]  timeout: Maximum duration to wait, in ms, if queue is full. 0 means do not wait and -1 means wait indefinitely.
    \param[in]  isr: Indicate if this is called from ISR. If set, @p timeout parameter is ignored.
    \param[out] none
    \retval     0 on success and != 0 if error occurred (i.e queue was full and maximum duration has been reached).
*/
int sys_queue_write(os_queue_t *queue, void *msg, int timeout, bool isr);

/*!
    \brief      read a message from a RTOS message queue.
    \param[in]  queue: pointer to the queue handle
    \param[in]  msg: Buffer to copy into. (It is assume that buffer is of the size specified in @ref os_queue_t_create)
    \param[in]  timeout: Maximum duration to wait, in ms, if queue is empty. 0 means do not wait and -1 means wait indefinitely.
    \param[in]  isr: Indicate if this is called from ISR. If set, @p timeout parameter is ignored.
    \param[out] none
    \retval     0 on success and != 0 if error occurred (i.e queue was empty and maximum duration has been reached).
*/
int sys_queue_read(os_queue_t *queue, void *msg, int timeout, bool isr);

/*!
    \brief      get the current system up time
    \param[in]  none
    \param[out] none
    \retval     milliseconds since the system boots up(0x00000000-0xffffffff)
*/
uint32_t sys_current_time_get(void);

/*!
    \brief      get the system time
    \param[in]  p: not used
    \param[out] none
    \retval     milliseconds since the system boots up(0x00000000-0xffffffff)
*/
uint32_t sys_time_get(void *p);

/*!
    \brief      put current task to sleep for a specified period of time
    \param[in]  ms: millisecond value, if the time is less than a tick, will roll up to 1 tick.
    \param[out] none
    \retval     none
*/
void sys_ms_sleep(int ms);

/*!
    \brief      delay the current task for several microseconds
                Note: The task will being blocked during the delayed time
    \param[in]  nus: microsecond value
    \param[out] none
    \retval     none
*/
void sys_us_delay(uint32_t nus);

/*!
    \brief      give up the execution of the current task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_yield(void);

/*!
    \brief      pend the task scheduling
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_sched_lock(void);

/*!
    \brief      resume the task scheduling
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_sched_unlock(void);

/*!
    \brief      Initialize the given buffer with the random data
    \param[in]  size: size of random data
    \param[out] dst: pointer to the get random bytes
    \retval     function run status
      \arg        OS_OK: run success
*/
int32_t sys_random_bytes_get(void *dst, uint32_t size);

/*!
    \brief      initialize a timer
    \param[in]  timer: pointer to the timer handle
    \param[in]  name: pointer to the timer name
    \param[in]  delay: the timeout in milliseconds
    \param[in]  periodic: whether it's periodic timer
    \param[in]  func: the timer call back function
    \param[in]  arg:the argument that will pass to the call back
    \param[out] none
    \retval     none
*/
void sys_timer_init(os_timer_t *timer, const uint8_t *name, uint32_t delay, uint8_t periodic,
                    timer_func_t func, void *arg);

/*!
    \brief      delete a timer
    \param[in]  timer: pointer to the timer handle
    \param[out] none
    \retval     none
*/
void sys_timer_delete(os_timer_t *timer);

/*!
    \brief       start a timer
    \param[in]  timer: pointer to the timer handle
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                  with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     none
*/
void sys_timer_start(os_timer_t *timer, uint8_t from_isr);

/*!
    \brief      start the timer extension with a specified timeout
    \param[in]  timer: pointer to the timer handle
    \param[in]  delay: time want to delay
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                  with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     none
*/
void sys_timer_start_ext(os_timer_t *timer, uint32_t delay, uint8_t from_isr);

/*!
    \brief      stop the timer
    \param[in]  timer: pointer to the timer handle
    \param[in]  from_isr: whether it's called from an ISR, it's only meaningful to some OS
                      with implementations seperated for different execution context, like FreeRTOS
    \param[out] none
    \retval     function run state 1: run ok, 0: have error happen
*/
uint8_t sys_timer_stop(os_timer_t *timer, uint8_t from_isr);

/*!
    \brief      check if the timer is active and pending for expiration
    \param[in]  timer: pointer to the timer handle
    \param[out] none
    \retval     1 if it's pending, 0 otherwise.
*/
uint8_t sys_timer_pending(os_timer_t *timer);

/*!
    \brief      initialize the OS
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_os_init(void);

/*!
    \brief      Miscellaneous initialization work after OS initialized and
                task scheduler started
                Note: It runs in the standalone start task
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_os_misc_init(void);

/*!
    \brief      start the OS
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_os_start(void);

/*!
    \brief      get the current RTOS time, in ticks.
    \param[in]  isr: Indicate if this is called from ISR.
    \param[out] none
    \retval     The current RTOS time (in ticks)
*/
uint32_t sys_os_now(bool isr);

/*!
    \brief      add heap region
    \param[in]  ucStartAddress: start address.
    \param[in]  xSizeInBytes: size in bytes.
    \param[out] none
    \retval     none
*/
void sys_add_heap_region(uint32_t ucStartAddress, uint32_t xSizeInBytes);

/*!
    \brief      remove heap region
    \param[in]  ucStartAddress: start address.
    \param[in]  xSizeInBytes: size in bytes.
    \param[out] none
    \retval     none
*/
void sys_remove_heap_region(uint32_t ucStartAddress, uint32_t xSizeInBytes);

/*!
    \brief      change the priority of a task
    \param[in]  task: Task handle.
    \param[in]  priority: priority to set to the task.
    \param[out] none
    \retval     none
*/
void sys_priority_set(void *task, os_prio_t priority);

/*!
    \brief      get the priority of a task
    \param[in]  task: Task handle.
    \param[out] priority to the task
    \retval     none
*/
os_prio_t sys_priority_get(void *task);

/*!
    \brief      rtos in critical
    \param[in]  none
    \param[out] none
    \retval     interrupt status in the critical nesting
*/
uint32_t sys_in_critical(void);

/*!
    \brief      rtos enter critical
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_enter_critical(void);

/*!
    \brief      rtos exit critical
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_exit_critical(void);

/*!
    \brief      set rtos power save mode
    \param[in]  mode
    \param[out] none
    \retval     none
*/
void sys_ps_set(uint8_t mode);

/*!
    \brief      get rtos power save mode
    \param[in]  none
    \param[out] none
    \retval     current rtos power mode
*/
uint8_t sys_ps_get(void);

/*!
    \brief      get cpu sleep time and stats time
    \param[out] stats_ms
    \param[out] sleep_ms
    \retval     none
*/
void sys_cpu_sleep_time_get(uint32_t *stats_ms, uint32_t *sleep_ms);

/*!
    \brief      show cpu usage percentage per task
    \retval     none
*/
void sys_cpu_stats(void);

#ifdef __cplusplus
}
#endif

#endif // __WRAPPER_OS_H
