#ifndef EXTENSIONENABLER_LUA_H
#define EXTENSIONENABLER_LUA_H

#ifdef _WIN32
    #include <windows.h>
    #define THREAD_HANDLE HANDLE
    #define MUTEX_TYPE CRITICAL_SECTION
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_FUNC_RET 0
    #define THREAD_SLEEP(ms) Sleep(ms)
    #define MUTEX_INIT(mutex) InitializeCriticalSection(&mutex)
    #define MUTEX_LOCK(mutex) EnterCriticalSection(&mutex)
    #define MUTEX_UNLOCK(mutex) LeaveCriticalSection(&mutex)
    #define MUTEX_DESTROY(mutex) DeleteCriticalSection(&mutex)
    #define THREAD_CREATE(handle, func, arg) \
        ((*(handle) = CreateThread(NULL, 0, func, arg, 0, NULL)) == NULL ? 1 : 0)
    #define THREAD_CLOSE(handle) CloseHandle(handle)
#else
    #include <pthread.h>
    #include <unistd.h>
    #define THREAD_HANDLE pthread_t
    #define MUTEX_TYPE pthread_mutex_t
    #define THREAD_FUNC void *
    #define THREAD_FUNC_RET NULL
    #define THREAD_SLEEP(ms) usleep((ms) * 1000)
    #define MUTEX_INIT(mutex) pthread_mutex_init(&mutex, NULL)
    #define MUTEX_LOCK(mutex) pthread_mutex_lock(&mutex)
    #define MUTEX_UNLOCK(mutex) pthread_mutex_unlock(&mutex)
    #define MUTEX_DESTROY(mutex) pthread_mutex_destroy(&mutex)
    #define THREAD_CREATE(handle, func, arg) \
        pthread_create(handle, NULL, func, arg)
    #define THREAD_CLOSE(handle) pthread_detach(*handle)
#endif

#endif
