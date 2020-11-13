#include "memory_manager.h"

int main(int argc, char **argv){

    std::list <MemoryBlock> allocMBList, freedMBList; // Data structures to manage allocated and free memory
    STARTING_PB = sbrk(0); // Find and initialise the current program break

    std::fstream file;
    std::ofstream outputFile;

    // Starting block to locate and open file
    try{
        if(argc < 3){
            throw CustomException("missing arguments, <allocation-strategy> <names.txt>");
        }
        std::string flag = argv[1];
        if(!(flag=="-f" || flag=="-b" || flag=="-w")){
            throw CustomException("you must select your allocation strategy; -f 'first' -b 'best' -w 'worst'");
        }
        if(flag == "-f")
            FILENAME_STRATEGY = "first-fit";
        if(flag == "-b")
            FILENAME_STRATEGY = "best-fit";
        if(flag == "-w")
            FILENAME_STRATEGY = "worst-fit";

        char const* const filename = argv[2];
        file.open(filename, std::ios::in);

        if (!file.is_open()){
            throw CustomException("could not open the file");
        }
        //Initialise first ALLOCATION_LIMIT to the heap
        allocateMemory(allocMBList, file, ALLOCATION_LIMIT);

        while(file.peek()!=EOF){
            deallocateMemory(allocMBList, freedMBList, DEALLOCATION_LIMIT);

            if(flag == "-f")
                firstFit(allocMBList, freedMBList, file, ALLOCATION_LIMIT);
            if(flag == "-b")
                bestFit(allocMBList, freedMBList, file, ALLOCATION_LIMIT);
            if(flag == "-w")
                worstFit(allocMBList, freedMBList, file, ALLOCATION_LIMIT);
        }
        file.close();

        //Output file section
        std::string fn(filename);
        fn = fn.substr(0, fn.find_last_of('.'));
        std::string outFilename = "output/" + FILENAME_STRATEGY + "-" + fn + ".csv";

        // void* finalPB = sbrk(0);
        // auto size = (char*)finalPB - (char*)STARTING_PB;

        //Create and open a csv
        outputFile.open(outFilename);

        //The total memory allocated using sbrk or brk.
        outputFile << "Total memory allocated by sbrk() [" << getMemoryAllocatedSize() << "]\n\n";
        outputFile << "[freedMBList]\n";
        outputFile << "Starting Address,Node Size\n";

        std::list<MemoryBlock>::iterator it;
        for(it = freedMBList.begin(); it != freedMBList.end(); ++it) {
            outputFile << (char**)it->address << "," << it->size << "\n";
        }
        outputFile << "\n\n[allocMBList]\n";
        outputFile << "Starting Address,Node Size,Content\n";

        for(it = allocMBList.begin(); it != allocMBList.end(); ++it) {
            outputFile << (char**)it->address << "," << it->size << "," << (char*)it->address << "\n";
        }

        outputFile.close();

    }catch(CustomException& e){
        std::cout << e.getMessage() << std::endl;
        file.close();
        outputFile.close();

        quit(EXIT_FAILURE);
    }
    quit(EXIT_SUCCESS);
}

/*
 * Add data from file into desired storage list, takes the argument limit which
 * allocates only a set amount of data from file.
 *
 * The function will return true if there is more data to be allocated.
 */
bool allocateMemory(std::list<MemoryBlock>& storage, std::fstream& file, int limit){
    bool response = false;

    int count = 0;
    while(file.peek()!=EOF && count < limit){ //peek() allows for conditional check leaving lines in file
        response = true;

        std::string name = getWord(file);
        int size = name.length();
        //Due to sbrk returning original program break we need to allow for one extra space
        void * ptr = sbrk(size+1);
        std::strcpy((char*)ptr,name.c_str()); //Point the address to the name given

        MemoryBlock * mb = new MemoryBlock((char*)ptr, size);
        storage.push_back(*mb);

        incrementMemoryCounter(size);

        count ++;
    }
    if(count < limit){ response = false; } //Need to check that we don't go over our input limit
    return response;
}

/*
 * Removes a random amount of memory blocks from the allocated memory block list then places the
 * recently deleted blocks into the free memory blocks list. The random number of blocks selected
 * is given as an int amount argument.
 *
 * The function will return true if there is still memory allocated
 */
bool deallocateMemory(std::list<MemoryBlock>& allocMBList, std::list<MemoryBlock>& freeMBList, int amount){
    bool response = false;

    std::srand(std::time(0)); //Seed a random time based on the current clock time
    int count = amount;
    while(!allocMBList.empty() && count > 0){
        response = true;
        //Generate a random number from previous seed; limit the number to the size of allocMB list
        int index = std::rand() % allocMBList.size();
        std::list<MemoryBlock>::iterator it = allocMBList.begin();

        std::advance(it, index); //Retrieve the node at given random index depth

        //Push and allocate a new memory block into the freeMB list
        *it->address = '\0';
        freeMBList.push_back(*new MemoryBlock(it->address, it->size));
        allocMBList.erase(it); // Remove the node from list
        count--;

        mergeBlocks(freeMBList); //Merge blocks once placed in free memory
    }
    if(allocMBList.empty()){ response = false; }
    return response;
}

void mergeBlocks(std::list<MemoryBlock>& freeMBList){
    std::list<MemoryBlock>::iterator it;
    for(it = freeMBList.begin(); it != freeMBList.end(); ++it){
        std::list<MemoryBlock>::iterator next = std::next(it);

        //Will locate the next memory block given the word size is the difference of the two pointers in bits
        if(next->address - it->address == (it->size+1)){
            it->size = it->size + next->size;
            freeMBList.erase(next);
        }
    }
}

/*
 *  Implementation of first fit algorithm: "find the first memory block in freedMBList that is big enough;
 *  you split the block and return to the user a chunk that is big enough to meet their needs. Then, you add
 *  the rest of the block back onto freedMBList."
 */
void firstFit(std::list<MemoryBlock>& allocMBList, std::list<MemoryBlock>& freeMBList, std::fstream& file, int limit){
    std::list<MemoryBlock>::iterator it;

    int count = 0;
    while(file.peek()!=EOF && count < limit){
        bool response = false; //Checking condition if a word has already been assigned from first fit

        std::string name = getWord(file); //Get name from line
        int size = name.length();

        if(!freeMBList.empty()){
            for(it = freeMBList.begin(); it != freeMBList.end(); ++it) {
                if(it->size >= size){ //Find a node with a greater or equal size
                    MemoryBlock mb = splitMemoryBlock(freeMBList, *it, size);
                    std::strcpy((char*)mb.address,name.c_str());
                    allocMBList.push_back(mb);
                    freeMBList.erase(it);
                    mergeBlocks(freeMBList);
                    response = true;
                    count++;
                }
                if(response){
                    break;
                }
            }
        }
        if(!response){ //If unable to find a free space in the freeMBList then alloc new space on the heap
            int size = name.length();
            void * ptr = sbrk(size+1);
            std::strcpy((char*)ptr,name.c_str());

            MemoryBlock * mb = new MemoryBlock((char*)ptr, size);
            allocMBList.push_back(*mb);

            incrementMemoryCounter(size);

            count++;

        }
    }
}
/*
 *  find the memory block in freedMBList whose size is the closest match to the allocation need;
 *  you split the block and return to the user a chunk that is big enough to meet their needs.
 *  Then, you add the rest of the block back onto freedMBList.
 */
void bestFit(std::list<MemoryBlock>& allocMBList, std::list<MemoryBlock>& freeMBList, std::fstream& file, int limit){
    int count = 0;
    while(file.peek()!=EOF && count < limit) {
        bool found = false;
        std::string name = getWord(file); //Get name from line
        int size = name.length();

        if (!freeMBList.empty()) {
            MemoryBlock bestNode = freeMBList.front();

            std::list<MemoryBlock>::iterator it;
            for (it = freeMBList.begin(); it != freeMBList.end(); ++it) {
                if(bestNode.size == size){
                    break;
                }
                if(bestNode.size < size){
                    bestNode = *it;
                }
                else if (it->size < bestNode.size && it->size >= size){
                    bestNode = *it;
                }
            }
            if(bestNode.size >= size){
                MemoryBlock mb = splitMemoryBlock(freeMBList, bestNode, size);
                std::strcpy((char*)mb.address,name.c_str());

                for (it = freeMBList.begin(); it != freeMBList.end(); ++it) {
                    if (*it->address == *mb.address) {
                        freeMBList.erase(it);
                        break;
                    }
                }
                mergeBlocks(freeMBList);
                allocMBList.push_back(mb);
                count++;
                found = true;
            }
        }
        if(!found){
            void * ptr = sbrk(size+1);
            std::strcpy((char*)ptr,name.c_str());

            MemoryBlock * mb = new MemoryBlock((char*)ptr, size);
            allocMBList.push_back(*mb);

            incrementMemoryCounter(size);

            count++;
        }
    }
}
/*
 *  find the largest memory block in freedMBList; you split the block and return to the user a chunk
 *  that is big enough to meet their needs. Then, you add the rest of the block back onto freedMBList.
 */
void worstFit(std::list<MemoryBlock>& allocMBList, std::list<MemoryBlock>& freeMBList, std::fstream& file, int limit){
    int count = 0;
    while(file.peek()!=EOF && count < limit) {
        count++;
        bool found = false;
        std::string name = getWord(file); //Get name from line
        int size = name.length();

        if (!freeMBList.empty()) {
            MemoryBlock bestNode = freeMBList.front();

            std::list<MemoryBlock>::iterator it;
            for (it = freeMBList.begin(); it != freeMBList.end(); ++it) {
                if (it->size > bestNode.size){
                    bestNode = *it;
                }
            }
            if(bestNode.size >= size) {
                MemoryBlock mb = splitMemoryBlock(freeMBList, bestNode, size);
                std::strcpy((char *) mb.address, name.c_str());
                allocMBList.push_back(mb);
                found = true;

                for (it = freeMBList.begin(); it != freeMBList.end(); ++it) {
                    if (*it->address == *mb.address) {
                        freeMBList.erase(it);
                        break;
                    }
                }
                mergeBlocks(freeMBList);
            }
        }
        if(!found) {
            void * ptr = sbrk(size+1);
            std::strcpy((char*)ptr,name.c_str());

            MemoryBlock * mb = new MemoryBlock((char*)ptr, size);
            allocMBList.push_back(*mb);

            incrementMemoryCounter(size);
        }
    }
}

/*
 *  Splits a memory block given desired size, used block will be returned, while leftover chunks
 *  will be placed back into the freeMBList
 */
MemoryBlock splitMemoryBlock(std::list<MemoryBlock>& freeMBList, MemoryBlock& block, int size){
    int remainingSize = block.size - size;
    if(remainingSize > 0){
        block.size = size;

        char * newAddress = block.address;
        for(int i=0; i<remainingSize; i++){
            newAddress++;
        }
        *newAddress = '\0';
        freeMBList.push_back(*new MemoryBlock((char*)newAddress, remainingSize));
        mergeBlocks(freeMBList);
    }
    return block;
}

std::string getWord(std::fstream& file){
    std::string text;
    getline(file, text);
    std::string name = text;

    if(name.find('\r') != std::string::npos)
        name.erase(std::remove(name.begin(), name.end(), '\r'), name.end());

    return name;
}

/*
 *  Function keeps track of user incremented program break
 */
void incrementMemoryCounter(int amount){
    MEMORY_ALLOCATED += amount;
}
int getMemoryAllocatedSize(){
    return MEMORY_ALLOCATED;
}

void quit(int success){
    exit(success);
}