/*
 * Copyright (C) 2023 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#pragma once

#include <semaphore.h>
#include <string>

namespace cxxsemaphore {

class Semaphore final {
private:
    const std::string NAME;                    //*< semaphore name
    sem_t            *semaphore = SEM_FAILED;  //*< posix semaphore
    bool              acquired  = false;       //*< indicates whether the instance acquired the semaphore
    const bool        owner;                   //*< true if the instance created the semaphore

public:
    /**
     * @brief Create a new named semaphore
     *
     * @param name semaphore name (leading '/' is implicitly set)
     * @param value semaphore value
     * @param force create semaphore even if a semaphore with the same name already exists
     */
    explicit Semaphore(const std::string name, unsigned int value, bool force = false);

    /**
     * @brief Open an existing named semaphore
     *
     * @param name semaphore name (leading '/' is implicitly set)
     */
    explicit Semaphore(const std::string name);

    ~Semaphore();

    /**
     * @brief wait for the semaphore
     */
    void wait();

    /**
     * @brief wait for the semaphore (with timeout)
     *
     * @param timeout timeout (relative to NOW)
     */
    [[nodiscard]] bool wait(const struct timespec &timeout);

    /**
     * @brief try to acquire the semaphore
     *
     * @return true semaphore acquired
     * @return false semaphore not available
     */
    [[nodiscard]] bool try_wait();

    /**
     * @brief post (release) the semaphore
     */
    void post();

    [[nodiscard]] inline bool is_acquired() const noexcept { return acquired; }

    [[nodiscard]] inline const std::string &get_name() const noexcept { return NAME; }
};

}  // namespace cxxsemaphore
