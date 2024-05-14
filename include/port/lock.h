//
// Created by hanyuan on 2024/4/23.
//

#ifndef TEMU_LOCK_H
#define TEMU_LOCK_H

#include "parameters.h"

#ifndef BARE_METAL_PLATFORM
#include <pthread.h>

#if defined(__APPLE__)
#define PORT_LOCK_T pthread_rwlock_t
#else
#define PORT_LOCK_T pthread_spinlock_t
#endif
#else
#define PORT_LOCK_T char
#endif

int port_lock_init(PORT_LOCK_T *lock);

void port_lock_unlock(PORT_LOCK_T *lock);

void port_lock_lock(PORT_LOCK_T *lock, char is_write);

#endif //TEMU_LOCK_H
