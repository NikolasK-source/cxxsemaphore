# CxxSemaphore

A C++ Library to handle POSIX named Semaphores.

## Usage

### Initialize cxxsemaphore::Semaphore

#### Option A: Create Semaphore

```c++
const std::string sem_name     = "example_sem";  // semaphore name
constexpr unsigned sem_value   = 1;              // initial semaphore value
constexpr bool force           = false;          // fail if semaphore already exists

// create semaphore
cxxsemaphore::Semaphore sem(sem_name, sem_value, force);
```

#### Option B: Connect to Existing Semaphore

```c++
const std::string sem_name = "example_sem";  // semaphore name

// use semaphore
cxxsemaphore::Semaphore sem(shm_name, read_only);
```

### Wait for Semaphore

#### Wait forever

```c++
sem.wait();
```

#### Wait with Timeout (sec, nsec)

```c++
bool success = sem.wait({2, 0});
```

#### Try to acquire semaphore

```c++
bool success = sem.try_wait();
```

### Release the Semaphore

```c++
sem.post()
```
