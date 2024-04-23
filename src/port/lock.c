//
// Created by hanyuan on 2024/4/23.
//

#include "port/lock.h"

#if defined(__APPLE__)

int port_lock_init(PORT_LOCK_T *lock) {
    return pthread_rwlock_init(lock, NULL);
}

void port_lock_unlock(PORT_LOCK_T *lock) {
    pthread_rwlock_unlock(lock);
}

void port_lock_lock(PORT_LOCK_T *lock, char is_write) {
    if (is_write) {
        pthread_rwlock_wrlock(lock);
    } else {
        pthread_rwlock_rdlock(lock);
    }
}

#elif defined(__linux__)

int port_lock_init(PORT_LOCK_T *lock) {
    return pthread_spin_init(lock, NULL);
}

void port_lock_unlock(PORT_LOCK_T *lock) {
    pthread_spin_unlock(lock);
}

void port_lock_lock(PORT_LOCK_T *lock, char is_write) {
    pthread_spin_lock(lock);
}

#endif
