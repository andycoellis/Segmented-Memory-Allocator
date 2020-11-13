#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <list>
#include <ctime>
#include <cstring>
#include <iterator>
#include <algorithm>

#include "memory_block.h"
#include "custom_exception.h"

bool allocateMemory(std::list<MemoryBlock>&, std::fstream&, int);

bool deallocateMemory(std::list<MemoryBlock>&, std::list<MemoryBlock>&, int);
void mergeBlocks(std::list<MemoryBlock>&);

void firstFit(std::list<MemoryBlock>&, std::list<MemoryBlock>&, std::fstream&, int);
void bestFit(std::list<MemoryBlock>&, std::list<MemoryBlock>&, std::fstream&, int);
void worstFit(std::list<MemoryBlock>&, std::list<MemoryBlock>&, std::fstream&, int);

MemoryBlock splitMemoryBlock(std::list<MemoryBlock>&, MemoryBlock&, int);
std::string getWord(std::fstream&);

void incrementMemoryCounter(int);
int getMemoryAllocatedSize();

void quit(int);

void * STARTING_PB = nullptr;
int ALLOCATION_LIMIT = 1000;
int DEALLOCATION_LIMIT = 500;
std::string FILENAME_STRATEGY;
int MEMORY_ALLOCATED = 0;

#endif //UNTITLED_MEMORY_MANAGER_H
