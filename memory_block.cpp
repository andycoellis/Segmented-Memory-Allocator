#include "memory_block.h"

MemoryBlock::MemoryBlock(char* address, int size){
    this->address = address;
    this->size = size;
}

MemoryBlock::~MemoryBlock() {
    this->address = nullptr;
}