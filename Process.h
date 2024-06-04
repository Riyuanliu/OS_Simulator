//
//  Process.hpp
//  OS simulation
//
//  Created by Riyuan Liu on 5/6/24.
//

#ifndef Process_h
#define Process_h

#include <stdio.h>
#include <vector>

class Process {
public:
    int PID;
    bool isZombie = false;
    bool isWaiting = false;
    std::vector<int> childPIDs;
    int parentPID = NO_PROCESS;
};

#endif /* Process_hpp */
