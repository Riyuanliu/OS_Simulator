//
//  SimOS.hpp
//  OS simulation
//
//  Created by Riyuan Liu on 5/6/24.
//

#ifndef SimOS_h
#define SimOS_h

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include "FileReadRequest.h"
#include "MemoryItem.h"
#include "Process.h"


class SimOS{

private:
    int numberOfDisks;
    unsigned long long amountOfRAM;
    unsigned int pageSize;
    int currentPID = 1;
    std::unordered_map<int, Process> processes;
    std::deque<int> readyQueue;
    std::vector<std::deque<FileReadRequest>> diskQueues;
    int currentCPU = NO_PROCESS;
    std::vector<FileReadRequest> currentFileRead;
    MemoryUsage memoryFrames;
    std::vector<int> lastUsedTime;
    std::unordered_set<int> waitque;
    
    
public:
    SimOS( int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize);
    void NewProcess( );
    void SimFork();
    void SimExit();
    void SimWait();
    void inque(int pid);
    void terminateProcessAndDescendants(int pid);
    void releaseMemory(int pid);
    void removeProcessFromTheSystem(int pid);
    void TimerInterrupt();
    void DiskReadRequest( int diskNumber, std::string fileName );
    void DiskJobCompleted( int diskNumber );
    void AccessMemoryAddress(unsigned long long address);
    int GetCPU( );
    std::deque<int> GetReadyQueue( );
    MemoryUsage GetMemory( );
    FileReadRequest GetDisk( int diskNumber );
    std::deque<FileReadRequest> GetDiskQueue( int diskNumber );
};

#endif /* SimOS_h */

