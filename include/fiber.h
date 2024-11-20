// SPDX-FileCopyrightText: 2012-2023 Brian Watling <brian@oxbo.dev>
// SPDX-License-Identifier: MIT

#ifndef _FIBER_FIBER_H_
#define _FIBER_FIBER_H_

#include <stdint.h>

#include "fiber_context.h"
#include "mpsc_fifo.h"
// Souparna's Implementation of fiber_key_t analogues to pthread_key_t
#define FIBER_KEYS_MAX 1024

typedef int fiber_state_t;

// Souparna's Implementation of fiber_key_t analogues to pthread_key_t
typedef unsigned int fiber_key_t;
extern void* __fiber_keys[FIBER_KEYS_MAX];
extern int __fiber_key_count;
struct fiber_manager;

#define FIBER_STATE_RUNNING (1)
#define FIBER_STATE_READY (2)
#define FIBER_STATE_WAITING (3)
#define FIBER_STATE_DONE (4)
#define FIBER_STATE_SAVING_STATE_TO_WAIT (5)

#define FIBER_DETACH_NONE (0)
#define FIBER_DETACH_WAIT_FOR_JOINER (1)
#define FIBER_DETACH_WAIT_TO_JOIN (2)
#define FIBER_DETACH_DETACHED (3)

typedef struct fiber {
  volatile fiber_state_t state;
  fiber_run_function_t run_function;
  void* param;
  uint64_t volatile id; /* not unique globally, only within this fiber instance.
                           used for joining */
  fiber_context_t context;
  _Atomic(void*) result;
  mpsc_fifo_node_t* volatile mpsc_fifo_node;
  _Atomic int detach_state;
  _Atomic(struct fiber*) join_info;
  void* volatile scratch;  // to be used by internal fiber mechanisms. be sure
                           // mechanisms do not conflict! (ie. only use scratch
                           // while a fiber is sleeping/waiting)
} fiber_t;

#ifdef __cplusplus
extern "C" {
#endif

#define FIBER_DEFAULT_STACK_SIZE (102400)
#define FIBER_MIN_STACK_SIZE (1024)

extern fiber_t* fiber_create(size_t stack_size, fiber_run_function_t run,
                             void* param);

extern fiber_t* fiber_create_no_sched(size_t stack_size,
                                      fiber_run_function_t run, void* param);

extern fiber_t* fiber_create_from_thread();

extern int fiber_join(fiber_t* f, void** result);

extern int fiber_tryjoin(fiber_t* f, void** result);

extern int fiber_yield();

extern int fiber_detach(fiber_t* f);

// Souparna's Implementation of fiber key
extern int fiber_key_create (fiber_key_t *__key);
/* Destroy KEY.  */
extern int fiber_key_delete (fiber_key_t __key) ;
/* Return current value of the thread-specific data slot identified by KEY.  */
extern void *fiber_getspecific (fiber_key_t __key) ;
/* Store POINTER in the thread-specific data slot identified by KEY. */
extern int fiber_setspecific(fiber_key_t __key, const void* __pointer);

#ifdef __cplusplus
}
#endif

#endif
