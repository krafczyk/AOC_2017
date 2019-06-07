CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++17 -fconcepts -O3 -Wall -Werror -Wextra -I utilities
#CXXFLAGS=-std=c++17 -fconcepts -g -Wall -Werror -Wextra -I utilities

all: ./bin/task_1 ./bin/task_2 ./bin/task_3 ./bin/task_4 ./bin/task_5 ./bin/day_4 ./bin/day_5 ./bin/day_6

./bin/% : ./src/%.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf ./bin/*
