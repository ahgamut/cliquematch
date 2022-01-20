CXXFLAGS = -I../src/cliquematch/ -std=c++11 -O3 -c -g3 -Wall -Wpedantic -Wextra -fno-omit-frame-pointer
LINKFLAGS= -fno-omit-frame-pointer
VALFLAGS = --tool=memcheck --leak-check=full --leak-resolution=high --show-leak-kinds=all --errors-for-leak-kinds=all

UTILS_FILES = $(wildcard ../src/cliquematch/detail/*.cpp)
BASE_FILES = $(wildcard ../src/cliquematch/detail/graph/*.cpp)
WT_FILES = $(wildcard ../src/cliquematch/detail/nwgraph/*.cpp)

BASEOBJS = build/main.o $(patsubst ../%.cpp,build/%.o,$(BASE_FILES) $(UTILS_FILES))
WTOBJS = build/nwmain.o $(patsubst ../%.cpp,build/%.o,$(WT_FILES) $(UTILS_FILES))

all: build/cm_base build/cm_nw

build/%.o: %.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

build/src/%.o: ../src/%.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

build/cm_base: $(BASEOBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

build/cm_nw: $(WTOBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

test: build/cm_base build/cm_nw cond-mat-2003.mtx
	valgrind $(VALFLAGS) ./build/cm_base cond-mat-2003.mtx 0
	valgrind $(VALFLAGS) ./build/cm_nw cond-mat-2003.mtx

run: build/cm_base build/cm_nw cond-mat-2003.mtx
	./build/cm_base cond-mat-2003.mtx 0
	./build/cm_nw cond-mat-2003.mtx

clean:
	rm -f build/cm_base build/cm_nw
	find ./build/ -name "*.o" -exec rm {} \;
