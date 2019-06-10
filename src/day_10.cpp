#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef size_t type;

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    size_t size = 256;
	ArgParse::ArgParser Parser("Day 10");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-s/--size", "Size of the list of numbers", &size);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);

    // Create and initialize the list.
    std::vector<type> list;
    for(size_t i = 0; i < size; ++i) {
        list.push_back(i);
    }

    std::vector<type> lengths;

    std::string line;
    if(!std::getline(infile, line)) {
        std::cerr << "Problem with the input file" << std::endl;
        return -1;
    }

    std::regex numbers_regex("[0-9]+");
    auto nums_begin = std::sregex_iterator(line.begin(), line.end(), numbers_regex);
    auto nums_end = std::sregex_iterator();
    for(std::sregex_iterator i = nums_begin; i != nums_end; ++i) {
        lengths.push_back(fetch_value<int>(i->str()));
    }

    if(verbose) {
        std::cout << "lengths received!" << std::endl;
        for(auto& v: lengths) {
            std::cout << v << std::endl;
        }
    }

    if(verbose) {
        std::cout << "initial number list" << std::endl;
        for(auto& v: list) {
            std::cout << v << std::endl;
        }
    }

    // Perform 
    type skip = 0;
    type it = 0;
    type current_pos = 0;
    while(it < lengths.size()) {
        size_t len = lengths[it];
        // Swap elements until we get into the middle.
        for(size_t I = 0; I < len/2; ++I) {
            size_t i = (current_pos+I)%size;
            size_t j = (current_pos+len-I-1)%size;
            type temp = list[i];
            list[i] = list[j];
            list[j] = temp;
        }
        if(verbose) {
            std::cout << "list update" << std::endl;
            for(auto& v: list) {
                std::cout << v << std::endl;
            }
        }

        // Move current_position
        current_pos = (current_pos+len+skip)%size;
        skip += 1;
        it += 1;
    }

    std::cout << "Task 1: product: " << list[0]*list[1] << std::endl;

	return 0;
}
