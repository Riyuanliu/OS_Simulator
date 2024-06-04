//
//  FileReadRequest.hpp
//  OS simulation
//
//  Created by Riyuan Liu on 5/6/24.
//

#ifndef FileReadRequest_h
#define FileReadRequest_h

#include <stdio.h>
#include <iostream>

struct FileReadRequest
{
    int  PID{0};
    std::string fileName{""};
};

#endif /* FileReadRequest_hpp */
