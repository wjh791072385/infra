/*******************************************************************************
* Description: c async_jobcache_v0
* Author     : wjh
* Create     : 2023/xx/xx
*
*******************************************************************************/
#ifndef __ASYNC_JOBCACHE_V0__
#define __ASYNC_JOBCACHE_V0__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct  jobcache jobcache_t;

jobcache_t*     jobcache_init();

void            jobcache_exit(jobcache_t *cache);

int             jobcache_insert(jobcache_t *cache, void *data, uint32_t len) ;

void            jobcache_register(jobcache_t *cache, int (*handle) (void *, void *cb_data, void (*cb_func)(int ret, void *cb_data)));

#ifdef __cplusplus
}
#endif

#endif