/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "cxxsemaphore.hpp"

#include <cassert>
#include <system_error>

constexpr const char *SEM_NAME = "cxxsemaphore_test_semaphore";

int main() {
    cxxsemaphore::Semaphore sem(SEM_NAME, 1, true);

    try {
        cxxsemaphore::Semaphore sem2(SEM_NAME, 1);
        return 1;
    } catch (std::system_error &) {}

    sem.wait();
    sem.post();

    auto a = sem.try_wait();
    assert(a);
    sem.post();

    sem.wait();

    try {
        sem.wait();
        return 2;
    } catch (std::logic_error &) {}

    sem.post();

    try {
        sem.post();
        return 3;
    } catch (std::logic_error &) {}


    cxxsemaphore::Semaphore sem3(SEM_NAME);
}
