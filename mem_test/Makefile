CXX=g++
CXXFLAGS = -I../src/cliquematch/ -O3 -c -g3 -Wall -fno-omit-frame-pointer -DSTACK_DFS=1 -DBENCHMARKING=1
LINKFLAGS= -fno-omit-frame-pointer
SRC_DIR = ../src/cliquematch/core
SRC_FILES = dfs_recurse.cpp dfs_stack.cpp gbits.cpp graph.cpp heuristic.cpp mmio.cpp other_dfs.cpp searchstate.cpp vertex.cpp
OBJS = $(patsubst %.cpp,%.o,$(SRC_FILES) main.cpp)

all: cm_base

main.o: main.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

cm_base: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

test: cm_base cond-mat-2003.mtx
	valgrind --tool=memcheck --leak-check=full --leak-resolution=high --show-leak-kinds=all --errors-for-leak-kinds=all ./cm_base cond-mat-2003.mtx 1
	valgrind --tool=memcheck --leak-check=full --leak-resolution=high --show-leak-kinds=all --errors-for-leak-kinds=all ./cm_base cond-mat-2003.mtx 0

run: cm_base cond-mat-2003.mtx
	./cm_base cond-mat-2003.mtx 1
	./cm_base cond-mat-2003.mtx 0

clean:
	rm cm_base
	rm  *.o
