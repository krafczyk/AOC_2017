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
	std::string pattern;
	bool verbose = false;
	size_t test_val;
	bool test_passed = false;
	ArgParse::ArgParser Parser("Task 1");
	Parser.AddArgument("-p/--pattern", "Pattern to use", &pattern);
	Parser.AddArgument("-t/--test-val", "Value to test against", &test_val, ArgParse::Argument::Optional, &test_passed);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	size_t digits[pattern.size()];
	for(size_t idx=0; idx < pattern.size(); ++idx) {
		digits[idx] = (size_t) std::atol(pattern.substr(idx,1).c_str());
	}

	size_t sum = 0;
	for(size_t idx=0; idx < pattern.size(); ++idx) {
		size_t next_idx = (idx+1)%pattern.size();
		if(digits[idx] == digits[next_idx]) {
			sum += digits[idx];
		}
	}

	std::cout << "Sum is " << sum << std::endl;
	if(test_passed) {
		if(sum == test_val) {
			std::cout << "Test Passed!" << std::endl;
		} else {
			std::cout << "Test Failed!" << std::endl;
		}
	}

	return 0;
}
