.default: all

all: osp

first: 
	./osp -f first-names-1.txt
	./osp -f middle-names.txt

best: 
	./osp -b first-names-1.txt
	./osp -b middle-names.txt

worst: 
	./osp -w first-names-1.txt
	./osp -w middle-names.txt

clean:
	rm -rf osp *.o *.dSYM

osp: memory_manager.o memory_block.o custom_exception.o
	g++ -Wall -std=c++11 -g -O -o $@ $^

%.o: %.cpp
	g++ -Wall -std=c++11 -g -O -c $^

build: 
	g++ -Wall -std=c++11 -O -o osp *.cpp
