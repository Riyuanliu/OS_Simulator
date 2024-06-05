# SimOS

SimOS is an operating system simulator designed to provide a realistic environment for studying and understanding OS concepts such as process management, memory management, CPU scheduling, and I/O operations.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Process Management**: Simulate the creation, forking, and termination of processes.
- **Memory Management**: Handle memory allocation and address space access.
- **CPU Scheduling**: Implement a simple ready queue for CPU scheduling.
- **I/O Management**: Manage disk read requests and handle I/O completion.
- **Interrupt Handling**: Simulate timer interrupts for process scheduling.
- **Resource Management**: Manage multiple disks and memory frames.

## Installation

To get started with SimOS, follow these steps:

1. **Clone the repository**:
    ```bash
    git clone https://github.com/yourusername/simos.git
    ```
2. **Navigate to the project directory**:
    ```bash
    cd simos
    ```
3. **Compile the project**:
    ```bash
    g++ -std=c++11 -o simos main.cpp SimOS.cpp
    ```

## Usage

To run the OS Simulator, use the following command:

```bash
./simos
```

## Public Methods

SimOS class provides the following public methods:

- SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize): Constructor to initialize the OS with a given number of disks, RAM size, and page size.
- void NewProcess(): Creates a new process.
- void SimFork(): Forks the current process.
- void SimExit(): Exits the current process.
- void SimWait(): Makes the current process wait.
- void inque(int pid): Adds a process to the ready queue.
- void terminateProcessAndDescendants(int pid): Terminates a process and all its descendants.
- void releaseMemory(int pid): Releases memory allocated to a process.
- void removeProcessFromTheSystem(int pid): Removes a process from the system.
- void TimerInterrupt(): Handles a timer interrupt.
- void DiskReadRequest(int diskNumber, std::string fileName): Requests a disk read operation.
- void DiskJobCompleted(int diskNumber): Handles the completion of a disk job.
- void AccessMemoryAddress(unsigned long long address): Accesses a memory address.
- int GetCPU(): Returns the ID of the process currently using the CPU.
- std::deque<int> GetReadyQueue(): Returns the current ready queue.
- MemoryUsage GetMemory(): Returns the current memory usage.
- FileReadRequest GetDisk(int diskNumber): Returns the current disk operation for a specific disk.
- std::deque<FileReadRequest> GetDiskQueue(int diskNumber): Returns the queue of disk operations for a specific disk.

## Configuration
The SimOS class is configurable through its constructor parameters:

- numberOfDisks: Number of disks to manage.
- amountOfRAM: Total amount of RAM in the system.
- pageSize: Size of each memory page.

Example initialization:
```cpp
SimOS simos(4, 8192, 256);
```



