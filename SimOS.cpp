// Riyuan Liu
#include "SimOS.h"
#include <iostream>

// Constructor
SimOS::SimOS(int numDisks, unsigned long long ram, unsigned int pSize)
        : numberOfDisks(numDisks),
          amountOfRAM(ram),
          pageSize(pSize),
          diskQueues(numDisks), // Initialize diskQueues vector with numDisks deques
          currentFileRead(numDisks)
{};

// Create a new process and add it to the ready queue or assign to CPU if it's free
void SimOS::NewProcess() {
    int pid = currentPID++;
    Process newProcess{pid};
    processes[pid] = newProcess;
    // get in ready que
    inque(pid);
}
void SimOS::inque(int pid){
    if(currentCPU==NO_PROCESS){
        currentCPU = pid;
    }else{
        readyQueue.push_back(pid);
    }
}
// Simulate process forking
void SimOS::SimFork() {
    if (currentCPU == NO_PROCESS) {
        throw std::logic_error("No process is currently running on the CPU");
    }

    int pid = currentPID++;
    Process child{pid};
    child.parentPID = currentCPU;
    processes[currentCPU].childPIDs.push_back(pid);
    processes[pid] = child;
    // in ready queue
    inque(pid);
}

void SimOS::removeProcessFromTheSystem(int pid) {
    // Remove from the process map
    processes.erase(pid);

    // Check and remove from the current CPU if it's running this process
    if (currentCPU == pid) {
        currentCPU = NO_PROCESS;
    }

    // Remove from readyQueue
    auto it = std::remove(readyQueue.begin(), readyQueue.end(), pid);
    readyQueue.erase(it, readyQueue.end());

    // Remove from each disk queue
    for (auto& queue : diskQueues) {
        for (auto it = queue.begin(); it != queue.end();) {
            if (it->PID == pid) {
                it = queue.erase(it);  // Efficiently erase and move iterator
            } else {
                ++it;
            }
        }
    }

    // Remove from memory usage records
    memoryFrames.erase(
        std::remove_if(memoryFrames.begin(), memoryFrames.end(),
                       [pid](const MemoryItem& item) { return item.PID == pid; }),
        memoryFrames.end());

    // Remove from wait queue
    waitque.erase(pid);

    // Check and remove from currentFileRead if it's involved in current I/O operations
    for (auto& request : currentFileRead) {
        if (request.PID == pid) {
            request.PID = NO_PROCESS;  // Set PID to NO_PROCESS indicating no process
            request.fileName = "";     // Clear the file name
        }
    }
    if(!readyQueue.empty()){
        currentCPU = readyQueue.front();
        readyQueue.pop_front();
    }
    for(int i = 0;i<currentFileRead.size();i++){
        if(currentFileRead[i].PID==NO_PROCESS && !diskQueues[i].empty()){
            currentFileRead[i] = diskQueues[i].front();
            //diskQueues[i].pop_front();
        }
    }
}

// Helper function to recursively terminate processes
void SimOS::terminateProcessAndDescendants(int pid) {
    // Retrieve the process to terminate it along with its descendants.
    Process& proc = processes[pid];

    // Release the memory used by this process immediately.
    releaseMemory(pid);

    // Recursively terminate all child processes and remove them from the system.
    for (int childPid : proc.childPIDs) {
        terminateProcessAndDescendants(childPid);
        removeProcessFromTheSystem(childPid);
    }

    // Handle termination based on parent's state.
    if (proc.parentPID != NO_PROCESS) {
        Process& parent = processes[proc.parentPID];
        if (parent.isWaiting) {
            // If the parent is waiting, reactivate the parent.
            waitque.erase(proc.parentPID);
            if (currentCPU != proc.parentPID) {
                inque(proc.parentPID);
            }
            parent.isWaiting = false;
            processes.erase(pid);  // Remove the process after handling.
        }
    } else {
        // If no parent, remove the process directly.
        processes.erase(pid);
    }
}

void SimOS::releaseMemory(int pid) {
    // Efficiently remove memory frames associated with the given PID.
    memoryFrames.erase(std::remove_if(memoryFrames.begin(), memoryFrames.end(),
                                      [pid](const MemoryItem& item) { return item.PID == pid; }),
                       memoryFrames.end());
}

void SimOS::SimExit() {
    // Ensure there's a process to exit.
    if (currentCPU == NO_PROCESS) {
        throw std::logic_error("No process is currently running on the CPU to exit.");
    }
    processes[currentCPU].isZombie = true;
    // Save the PID of the current process and check for a parent.
    int pidToExit = currentCPU;
    int parentPID = processes[pidToExit].parentPID;

    // Initiate termination and clean-up.
    terminateProcessAndDescendants(pidToExit);

    // Handle post-termination logic based on the parent's state.
    if (parentPID != NO_PROCESS && processes.find(parentPID) != processes.end()) {
        Process& parent = processes[parentPID];
        if (parent.isWaiting) {
            // Reactivate the waiting parent.
            parent.isWaiting = false;
            inque(parentPID);
            processes.erase(pidToExit);  // Remove the process if the parent was waiting.
        }
    } else {
        // If no parent, or parent not found, remove the process.
        processes.erase(pidToExit);
    }

    // Reset the CPU and assign a new process if available.
    currentCPU = NO_PROCESS;
    if (!readyQueue.empty()) {
        currentCPU = readyQueue.front();
        readyQueue.pop_front();
    }
}


// Simulate waiting for child processes
void SimOS::SimWait() {
    if (currentCPU == NO_PROCESS) {
        throw std::logic_error("No process is currently running on the CPU to wait");
    }
    
    processes[currentCPU].isWaiting = true;
    if(!processes[currentCPU].childPIDs.empty()){
        for(int i  = 0;i<processes[currentCPU].childPIDs.size();i++){
            if (processes[processes[currentCPU].childPIDs[i]].isZombie) {
                terminateProcessAndDescendants(processes[currentCPU].childPIDs[i]);
                //swap with the last element and pop_back
                std::swap(processes[currentCPU].childPIDs[i], processes[currentCPU].childPIDs[processes[currentCPU].childPIDs.size()-1]);
                processes[currentCPU].childPIDs.pop_back();
                processes[currentCPU].isWaiting = false;
                return;
            }
        }
        waitque.insert(currentCPU);
        if(!readyQueue.empty()){
            currentCPU = readyQueue.front();
            readyQueue.pop_front();
            return;
        }
    }
    //if it dont got a kid, just do return;
    processes[currentCPU].isWaiting = false;
    
}

// Simulate timer interrupt
void SimOS::TimerInterrupt() {
    if (currentCPU == NO_PROCESS) return;
    
    readyQueue.push_back(currentCPU);
    currentCPU = readyQueue.front();
    readyQueue.pop_front();

}

void SimOS::DiskReadRequest(int diskNumber, std::string fileName) {
    // Check if the disk number is within valid range
    if (diskNumber < 0 || diskNumber >= numberOfDisks) {
        throw std::out_of_range("Invalid disk number");
    }
    if (currentCPU == NO_PROCESS) {
        throw std::logic_error("No process is currently running on the CPU to perform disk read");
    }

    // Create a new file read request with the current CPU's PID and the specified file name
    FileReadRequest request;
    request.PID = currentCPU;  // Assuming currentCPU holds the PID of the process currently using the CPU
    request.fileName = fileName;

    // Add the request to the appropriate disk queue
    if(currentFileRead[diskNumber].PID == NO_PROCESS){
        currentFileRead[diskNumber] = request;
    }else{
        diskQueues[diskNumber].push_back(request);
    }
    
    // Remove the current process from the CPU and set CPU to no process
    
    currentCPU = NO_PROCESS;
    if(!readyQueue.empty()){
        currentCPU = readyQueue.front();
        readyQueue.pop_front();
    }

    // Log or debug output
    //std::cout << "Process " << request.PID << " has requested to read file '"<< request.fileName << "' from disk " << diskNumber << std::endl;
}

void SimOS::DiskJobCompleted(int diskNumber) {
    // Check if the disk number is within valid range
    if (diskNumber < 0 || diskNumber >= numberOfDisks) {
        throw std::out_of_range("Invalid disk number");
    }

    // Check if there are any jobs on the disk
    if (diskQueues[diskNumber].empty() && currentFileRead[diskNumber].PID == NO_PROCESS) {
        std::cerr << "No jobs to complete on disk " << diskNumber << std::endl;
    }

    // Get the PID from the front of the queue and move it to the ready queue
    int pid = currentFileRead[diskNumber].PID;
    currentFileRead[diskNumber].PID = NO_PROCESS;
    currentFileRead[diskNumber].fileName = "";
    if(!diskQueues[diskNumber].empty()){
        currentFileRead[diskNumber] = diskQueues[diskNumber].front();
        diskQueues[diskNumber].pop_front();
    }
    
    readyQueue.push_back(pid);
    if(currentCPU ==NO_PROCESS){
        currentCPU = readyQueue.front();
        readyQueue.pop_front();
    }
    // Log or debug output
    //std::cout << "Disk job completed for process " << pid << " on disk " << diskNumber << ". Process moved to ready queue." << std::endl;
}



// Access a memory address and ensure the page is loaded
void SimOS::AccessMemoryAddress(unsigned long long address) {
    if (currentCPU == NO_PROCESS) {
        throw std::logic_error("No process is currently running on the CPU.");
    }

    unsigned long long pageNumber = address / pageSize;
    int frameIndex = -1;

    // Search for the page in the current memory frames
    for (size_t i = 0; i < memoryFrames.size(); ++i) {
        if (memoryFrames[i].pageNumber == pageNumber && memoryFrames[i].PID == currentCPU) {
            frameIndex = i;
            break;
        }
    }

    if (frameIndex != -1) {
        // If the page is found, move it to the back of the vector to mark it as recently used
        MemoryItem item = memoryFrames[frameIndex];
        memoryFrames.erase(memoryFrames.begin() + frameIndex);
        memoryFrames.push_back(item);
    } else {
        // If page is not found in RAM, we need to load it
        if (memoryFrames.size() < (amountOfRAM / pageSize)) {
            // There is a free frame in memory
            MemoryItem newItem{pageNumber, static_cast<unsigned long long>(memoryFrames.size()), currentCPU};
            memoryFrames.push_back(newItem);
        } else {
            // No free frame available, replace the least recently used frame (at the front of the vector)
            MemoryItem& lruItem = memoryFrames.front();
            lruItem.pageNumber = pageNumber;
            lruItem.PID = currentCPU;
            // Move the updated frame to the back to mark it as recently used
            MemoryItem updatedItem = lruItem;
            memoryFrames.erase(memoryFrames.begin());
            memoryFrames.push_back(updatedItem);
        }
    }
}


// Get the current CPU process ID
int SimOS::GetCPU() {
    return currentCPU;
}

std::deque<int> SimOS::GetReadyQueue() {
    return readyQueue;
}

MemoryUsage SimOS::GetMemory() {
    return memoryFrames;
}

FileReadRequest SimOS::GetDisk(int diskNumber) {
    if (diskNumber < 0 || diskNumber >= numberOfDisks) {
        throw std::out_of_range("Invalid disk number");
    }
    return currentFileRead[diskNumber];
}

std::deque<FileReadRequest> SimOS::GetDiskQueue(int diskNumber) {
    if (diskNumber < 0 || diskNumber >= numberOfDisks) {
        throw std::out_of_range("Invalid disk number");
    }
    return diskQueues[diskNumber];
}


