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

void report(const std::vector<size_t>& buffer, size_t i) {
    for(size_t idx = 0; idx < buffer.size(); ++idx) {
        if(idx == i) {
            std::cout << "(" << buffer[idx] << ")";
        } else {
            std::cout << " " << buffer[idx] << " ";
        }
    }
    std::cout << std::endl;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    size_t jump_val = 0;
    size_t limit = 0;
    bool limit_given = false;
	ArgParse::ArgParser Parser("Task Template");
	Parser.AddArgument("-j/--jump-val", "value for the jump", &jump_val);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-l/--limit", "Limit to do", &limit, ArgParse::Argument::Optional, &limit_given);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    std::vector<size_t> buffer = {0};

    size_t n = 1;
    size_t i = 0;
    while(n <= 2017) {
        if(limit_given) {
            if(n > limit) {
                break;
            }
        }
        if(verbose) {
            report(buffer, i);
        }
        // Advance to the new position
        i = (i+jump_val)%buffer.size();
        buffer.insert(buffer.begin()+i+1,n);
        i += 1;
        n += 1;
    }

    if(verbose) {
        report(buffer, i);
    }

    std::cout << "Task 1: value following 2017: " << buffer[i+1] << std::endl;

	return 0;
}
