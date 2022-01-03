CXXFLAGS = -I../src/cliquematch/ -std=c++11 -O3 -c -g3 -Wall -Wpedantic -fno-omit-frame-pointer
LINKFLAGS= -fno-omit-frame-pointer
VALFLAGS = --tool=memcheck --leak-check=full --leak-resolution=high --show-leak-kinds=all --errors-for-leak-kinds=all
SRC_DIR = ../src/cliquematch/detail
SRC_FILES = clique.cpp dfs_recurse.cpp dfs_stack.cpp gbits.cpp graph.cpp heuristic.cpp mmio.cpp vertex.cpp enumerate.cpp
OBJS = $(patsubst %.cpp,build/%.o,$(SRC_FILES) main.cpp)

all: build/cm_base

build/main.o: main.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

build/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

build/cm_base: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

test: build/cm_base cond-mat-2003.mtx
	valgrind $(VALFLAGS) ./build/cm_base cond-mat-2003.mtx 0

run: build/cm_base cond-mat-2003.mtx
	./build/cm_base cond-mat-2003.mtx 0

clean:
	rm -f build/cm_base build/*.o
