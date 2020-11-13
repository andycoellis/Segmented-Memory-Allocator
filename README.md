## Operating System Assessment 2
### Segmented Memory Allocator

**REDACTED University**

**Author Details:**
* Name: *Andrew Ellis*

### Overview
The code supplied is setup to run experiments on supplied text files.
These files are a list of names that are intended to be allocated to memory blocks
that sit within the heaps program break that is manipulated by the
programs system call of srbrk().

Additionally, to see the results of the experiment the program outputs
into a folder with csv's of performance data.

Specs:

> All code is of my own design based off intuition and course 
teachings that have been outlined in the assignment specs.

**All code was tested and run with C++11 on GNU/Linux x86_64**

### Setup

*The code found inside is intended to be compliled and run with a 
supplied Makefile, please make use of this as in order to efficiently
run the program*

##Note: 

**If you intend to use your own input text files please follow a subset of commands.**

* Be sure to place .txt files only in the same directory level as `memory_manager.cpp`
* Run `make build`
* Executable will be named `osp`
* choose either of three flags -f *first fit* -b *best fit* -w *worst fit*

*Example of running a best-fit experiment on a sample.txt file*

1. `make build`
2. `./osp -f sample.txt`

*Otherwise automation from a make file is below*

```Makefile``` commands:

* `make build` compile all source code and build project
* `make first` this runs the **first fit** allocation strategy with both text
files *first-names-1.txt* and *middle-names.txt*
* `make best` this runs the **best fit** allocation strategy with both text
files *first-names-1.txt* and *middle-names.txt*
* `make worst` this runs the **worst fit** allocation strategy with both text
files *first-names-1.txt* and *middle-names.txt*
* `make clean` cleans up all .o code and executables


### Contents of project

**Code:**
* `memory_manager.cpp` main driver of program, this is where you
can find the main method to run. It also handles all allocation 
strategies 
* `memory_manager.h`
* `memory_block.cpp` Light weight class create the memory block object
* `memory_block.h`
* `custom_exception.cpp` Inherits from `<exception>`, simple custom exception
class for throwing specific errors.
* `custom_exception.h`

**Folders**
* `output` folder to store csv's of individual strategy outputs

**Files:**
* `first-names-1.txt` a list of names that is used in the program as data source
* `middle-names.txt` same as above

### Outputs

Performance data will be outputted to the output folder in the directory.
It can be identified as `<allocation-strategy><input-filename>.csv`.

The files will output:
* First: Total heap size used in allocation strategy
* Break
* Freed Memory Block List Header
* `LOOP` -> FreedMB Node Pointer, FreedMB Node Size
*Break
* Allocated MB List Header
* `LOOP` -> AllocMB Node Pointer, AlllocMB Node Size, AllocMB Content Name   
