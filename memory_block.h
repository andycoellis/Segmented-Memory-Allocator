#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

#include <string>

class MemoryBlock {

public:
    MemoryBlock(char*, int);
    ~MemoryBlock();

    char* address;
    int size;
};


#endif //MEMORY_BLOCK_H
