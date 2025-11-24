#include "pocketpy/common/threads.h"
#include "pocketpy/common/utils.h"

#if PK_ENABLE_THREADS

#if PK_USE_PTHREADS

bool c11_thrd__create(c11_thrd_t* thrd, c11_thrd_func_t func, void* arg) {
    int res = pthread_create(thrd, NULL, func, arg);
    return res == 0;
}

void c11_thrd__yield() { sched_yield(); }

#else

bool c11_thrd__create(c11_thrd_t* thrd, c11_thrd_func_t func, void* arg) {
    int res = thrd_create(thrd, func, arg);
    return res == thrd_success;
}

void c11_thrd__yield() { thrd_yield(); }

#endif

static c11_thrd_retval_t _thrdpool_worker(void* arg) {
    while (true) {
        c11_thrd__yield();
    }
}

void c11_thrdpool__ctor(c11_thrdpool* pool, int length) {
    pool->length = length;
    pool->workers = PK_MALLOC(sizeof(c11_thrdpool_worker) * length);
    for (int i = 0; i < length; i++) {
        bool ok = c11_thrd__create(&pool->workers[i].thread, _thrdpool_worker, NULL);
        c11__rtassert(ok);
        pool->workers[i].arg = NULL;
        atomic_store(&pool->workers[i].is_idle, true);
    }
}

void c11_thrdpool__dtor(c11_thrdpool* pool) {
    for (int i = 0; i < pool->length; i++) {
        // Note: No proper thread joining or termination is done here.
        // This is a placeholder for demonstration purposes.
    }
    PK_FREE(pool->workers);
}

bool c11_thrdpool__create(c11_thrdpool* pool, c11_thrd_func_t func, void* arg) {
    // find the 1st idle worker
    for (int i = 0; i < pool->length; i++) {
        bool expected = true;
        if (atomic_compare_exchange_strong(&pool->workers[i].is_idle, &expected, false)) {
            // Found an idle worker, assign the task
            // Note: In a real implementation, you would need a way to pass the function and argument to the worker thread.
            // This is a placeholder for demonstration purposes.
            return true;
        }
    }
    return false;  // No idle worker found
}


#endif  // PK_ENABLE_THREADS