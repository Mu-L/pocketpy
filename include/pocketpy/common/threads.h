#pragma once

#include "pocketpy/config.h"

#if PK_ENABLE_THREADS

#include <stdatomic.h>
#include <stdbool.h>

#if __EMSCRIPTEN__ || __APPLE__ || __linux__
#include <pthread.h>
#define PK_USE_PTHREADS 1
typedef pthread_t c11_thrd_t;
typedef void* c11_thrd_retval_t;
#else
#include <threads.h>
#define PK_USE_PTHREADS 0
typedef thrd_t c11_thrd_t;
typedef int c11_thrd_retval_t;
#endif

typedef c11_thrd_retval_t (*c11_thrd_func_t)(void*);

bool c11_thrd__create(c11_thrd_t* thrd, c11_thrd_func_t func, void* arg);
void c11_thrd__yield();


typedef struct c11_thrdpool_worker {
    c11_thrd_t thread;
    void* arg;
    atomic_bool is_idle;
} c11_thrdpool_worker;

typedef struct c11_thrdpool {
    int length;
    c11_thrdpool_worker* workers;
} c11_thrdpool;

void c11_thrdpool__ctor(c11_thrdpool* pool, int length);
void c11_thrdpool__dtor(c11_thrdpool* pool);
bool c11_thrdpool__create(c11_thrdpool* pool, c11_thrd_func_t func, void* arg);

#endif