CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++17 -fconcepts -O3 -Wall -Werror -Wextra -I utilities
#CXXFLAGS=-std=c++17 -fconcepts -g -Wall -Werror -Wextra -I utilities

all: ./bin/task_1 ./bin/task_2 ./bin/task_3 ./bin/task_4 ./bin/task_5 ./bin/day_4 ./bin/day_5 ./bin/day_6 ./bin/day_7 ./bin/day_8 ./bin/day_9 ./bin/day_10 ./bin/day_11 ./bin/day_12 ./bin/day_13 ./bin/day_14 ./bin/day_15 ./bin/day_16 ./bin/day_17 ./bin/day_18 ./bin/day_18_decompile ./bin/day_19 ./bin/day_20

./bin/% : ./src/%.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@

./bin/day_18_decompile: ./src/day_18_decompile.cpp
	$(CXX) $(CXXFLAGS) -pthread $< -o $@

clean:
	rm -rf ./bin/*
