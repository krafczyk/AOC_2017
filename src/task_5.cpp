#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef int type;
typedef std::pair<type,type> point;

std::vector<point> dirs = { {0,1}, {-1,0}, {0,-1}, {1,0}};

point val_pos(type val) {
    // First find first N^2 which is >=p

    type N = 0;
    while((2*N+1)*(2*N+1) < val) {
        N += 1;
    }

    // Amount from start of outer ring.
    type n = val - (2*N-1)*(2*N-1);

    std::cout << "number of positions to go: " << n << std::endl;

    point position(N,-N);

    for(const point& dir: dirs) {
        type steps = 0;
        if(n < 2*N) {
            steps = n;
        } else {
            steps = 2*N;
        }
        position = point(position.first+steps*dir.first,position.second+steps*dir.second);
        n -= steps;
        if(n == 0) {
            break;
        }
    }
    return position;
}

int main(int argc, char** argv) {
	// Parse Arguments
	type array_position;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task Template");
	Parser.AddArgument("-i/--input", "Array position", &array_position);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    // First find first N^2 which is >=p

    point position = val_pos(array_position);

    std::cout << "final position: " << position.first << "," << position.second << std::endl;
    std::cout << "steps required to carry: " << (std::abs(position.first)+std::abs(position.second)) << std::endl;

	return 0;
}
