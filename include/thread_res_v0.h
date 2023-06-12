/*******************************************************************************
* Description: c resident thread
* Author     :
* Create     : 2023/xx/xx
*
*******************************************************************************/

#ifndef __THREAD_RES_V0__
#define __THREAD_RES_V0__

#include <pthread.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct thread_res thread_res_t;

thread_res_t* thread_res_create(void *cb_data, void (*cb_func) (void *),
                                bool (*need_sleep) (void *), void (*clean_up) (void *));

void thread_res_destory(thread_res_t *);

void thread_res_wakeup(thread_res_t *);


#ifdef __cplusplus
}
#endif

#endif