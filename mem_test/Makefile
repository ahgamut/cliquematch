CXXFLAGS = -I../src/cliquematch/ -std=c++11 -O3 -c -g3 -Wall -Wpedantic -fno-omit-frame-pointer
LINKFLAGS= -fno-omit-frame-pointer
VALFLAGS = --tool=memcheck --leak-check=full --leak-resolution=high --show-leak-kinds=all --errors-for-leak-kinds=all
SRC_DIR = ../src/cliquematch/detail
SRC_FILES = clique.cpp dfs_recurse.cpp dfs_stack.cpp gbits.cpp graph.cpp heuristic.cpp mmio.cpp vertex.cpp enumerate.cpp
OBJS = $(patsubst %.cpp,%.o,$(SRC_FILES) main.cpp)

all: cm_base

main.o: main.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

cm_base: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

test: cm_base cond-mat-2003.mtx
	valgrind $(VALFLAGS) ./cm_base cond-mat-2003.mtx 0

run: cm_base cond-mat-2003.mtx
	./cm_base cond-mat-2003.mtx 0

clean:
	rm cm_base
	rm  *.o
