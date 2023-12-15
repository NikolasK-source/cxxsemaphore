/*
 * Copyright (C) 2023 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "cxxsemaphore.hpp"

#include <fcntl.h>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/time.h>
#include <system_error>

namespace cxxsemaphore {

Semaphore::Semaphore(const std::string name, unsigned int value, bool force) : NAME(std::move(name)), owner(true) {
    if (value == 0) throw std::invalid_argument("semaphore cannot be initialized with value 0");

    int flag = O_CREAT;
    if (!force) flag |= O_EXCL;

    semaphore = sem_open(("/" + NAME).c_str(), flag, 0640, value);

    if (semaphore == SEM_FAILED)
        throw std::system_error(errno, std::generic_category(), "Failed to open semaphore '" + NAME + '\'');
}

Semaphore::Semaphore(const std::string name) : NAME(std::move(name)), owner(false) {
    semaphore = sem_open(("/" + NAME).c_str(), 0);

    if (semaphore == SEM_FAILED)
        throw std::system_error(errno, std::generic_category(), "Failed to open semaphore '" + NAME + '\'');
}

Semaphore::~Semaphore() {
    auto tmp = sem_close(semaphore);
    if (tmp == -1) perror(("sem_close failed for semaphore '" + NAME + '\'').c_str());

    if (owner) {
        tmp = sem_unlink(("/" + NAME).c_str());
        if (tmp == -1) perror(("sem_unlink failed for semaphore '" + NAME + '\'').c_str());
    }
}

void Semaphore::wait() {
    if (acquired) throw std::logic_error("semaphore already acquired");

    auto tmp = sem_wait(semaphore);
    if (tmp == -1)
        throw std::system_error(errno, std::generic_category(), "Failed to sem_wait for semaphore '" + NAME + '\'');

    acquired = true;
}

bool Semaphore::wait(const struct timespec &timeout) {
    if (acquired) throw std::logic_error("semaphore already acquired");

    if (timeout.tv_sec < 0 || timeout.tv_nsec < 0 || timeout.tv_nsec >= 1'000'000 ||
        (timeout.tv_sec == 0 && timeout.tv_nsec == 0))
        throw std::invalid_argument("invalid timeout value");

    struct timeval current_time;
    auto           tmp = gettimeofday(&current_time, nullptr);
    if (tmp == -1) throw std::system_error(errno, std::generic_category(), "gettimeofday");

    struct timespec timeout_time;
    timeout_time.tv_sec  = timeout.tv_sec + current_time.tv_sec;
    timeout_time.tv_nsec = timeout.tv_nsec + current_time.tv_usec * 1'000;

    timeout_time.tv_sec += timeout_time.tv_nsec / 1'000'000'000;
    timeout_time.tv_nsec %= 1'000'000'000;

    tmp = sem_timedwait(semaphore, &timeout_time);
    if (tmp == -1) {
        if (errno == ETIMEDOUT) return false;
        throw std::system_error(
                errno, std::generic_category(), "Failed to sem_timedwait for semaphore '" + NAME + '\'');
    }

    acquired = true;
    return true;
}

bool Semaphore::try_wait() {
    if (acquired) throw std::logic_error("semaphore already acquired");

    auto tmp = sem_trywait(semaphore);
    if (tmp == -1) {
        if (errno == EAGAIN) return false;
        throw std::system_error(errno, std::generic_category(), "Failed to sem_trywait for semaphore '" + NAME + '\'');
    }

    acquired = true;
    return true;
}

void Semaphore::post() {
    if (!acquired) throw std::logic_error("semaphore not acquired");

    auto tmp = sem_post(semaphore);
    if (tmp == -1)
        throw std::system_error(errno, std::generic_category(), "Failed to sem_post for semaphore '" + NAME + '\'');

    acquired = false;
}

}  // namespace cxxsemaphore
