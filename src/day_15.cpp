#include <iomanip>
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

inline void gen(int64_t& val, int64_t& fac) {
    val = (val*fac)%2147483647;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 15");
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

    std::unordered_map<std::string,int64_t> initial_values;

    std::regex input_match("^Generator ([AB]) starts with ([0-9]+)$");
    std::string line;
    while(std::getline(infile, line)) {
        std::smatch match;
        if(!std::regex_match(line, match, input_match)) {
            std::cerr << "There was a problem with the input!" << std::endl;
            return -1;
        }
        initial_values[match[1].str()] = fetch_value<int64_t>(match[2].str());
    }

    bool found_a = false;
    bool found_b = false;
    for(auto& p: initial_values) {
        if(p.first == "A") {
            found_a = true;
        }
        if(p.first == "B") {
            found_b = true;
        }
    }
    if(!(found_a&&found_b)) {
        std::cerr << "Didn't receive all starting values!" << std::endl;
        return -1;
    }

    size_t num_matches = 0;
    int64_t& gen_a = initial_values["A"];
    int64_t& gen_b = initial_values["B"];
    int64_t fac_a = 16807;
    int64_t fac_b = 48271;
    for(size_t i = 0; i < 40000000; ++i) {
        // Generate next values
        gen(gen_a, fac_a);
        gen(gen_b, fac_b);
        // Judge them
        if(gen_a%(65536)==gen_b%(65536)) {
            num_matches += 1;
        }
    }

    std::cout << "The judge found " << num_matches << " of matches" << std::endl;

	return 0;
}
