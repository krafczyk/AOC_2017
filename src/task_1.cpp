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
	bool pattern_passed = false;
	std::string input_filepath;
	bool input_filepath_passed = false;
	bool verbose = false;
	size_t test_val;
	bool test_passed = false;
	ArgParse::ArgParser Parser("Task 1");
	Parser.AddArgument("-p/--pattern", "Pattern to use", &pattern, ArgParse::Argument::Optional, &pattern_passed);
	Parser.AddArgument("-i/--input", "Input file to use", &input_filepath, ArgParse::Argument::Optional, &input_filepath_passed);
	Parser.AddArgument("-t/--test-val", "Value to test against", &test_val, ArgParse::Argument::Optional, &test_passed);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	if((!pattern_passed)&&(!input_filepath_passed)) {
		std::cerr << "You need to pass input of some type!" << std::endl;
		return -1;
	}

	if((pattern_passed)&&(input_filepath_passed)) {
		std::cerr << "Please only pass one type of input!" << std::endl;
		return -1;
	}

	std::vector<size_t> digits;

	if(pattern_passed) {
		for(size_t idx=0; idx < pattern.size(); ++idx) {
			digits.push_back((size_t) std::atol(pattern.substr(idx,1).c_str()));
		}
	}

	if(input_filepath_passed) {
		std::ifstream infile(input_filepath);
		std::string line;
		// Get only a single line.
		std::getline(infile, line);

		for(size_t idx = 0; idx < line.size(); ++idx) {
			digits.push_back((size_t) std::stol(line.substr(idx,1).c_str()));
		}
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
