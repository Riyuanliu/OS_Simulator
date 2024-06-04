//
//  MemoryItem.hpp
//  OS simulation
//
//  Created by Riyuan Liu on 5/6/24.
//

#ifndef MemoryItem_h
#define MemoryItem_h

#include <stdio.h>
#include <vector>

struct MemoryItem
{
    unsigned long long pageNumber;
    unsigned long long frameNumber;
    int PID; // PID of the process using this frame of memory
};
 
using MemoryUsage = std::vector<MemoryItem>;
 
constexpr int NO_PROCESS{ 0 };

#endif /* MemoryItem_hpp */
