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

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task Template");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);

    std::vector<int> jumps;

    std::string line;
    while(std::getline(infile, line)) {
        jumps.push_back(fetch_value<int>(line));
    }

    if(verbose) {
        std::cout << "Read jumps: " << std::endl;
        for(auto& i : jumps) {
            std::cout << i << std::endl;
        }
    }

    std::vector<int> jumps_copy = jumps;

    int inst = 0;
    int num = 0;
    do {
        inst += (jumps_copy[inst]++);
        num += 1;
    } while(inst < (int)jumps_copy.size());

    std::cout << "Part 1: There were " << num << " instructions executed" << std::endl;

    inst = 0;
    num = 0;
    do {
        int prev_inst = inst;
        int offset = jumps[inst];
        inst += offset;
        num += 1;
        if(offset >= 3) {
            jumps[prev_inst] -= 1;
        } else {
            jumps[prev_inst] += 1;
        }
    } while(inst < (int)jumps.size());

    std::cout << "Part 2: There were " << num << " instructions executed" << std::endl;
	return 0;
}
