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



int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    int size = 256;
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

    std::vector<int> list;
    std::vector<int> lengths;

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

	return 0;
}
