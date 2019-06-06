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
	int test_val = 0;
	bool test_passed = false;
	ArgParse::ArgParser Parser("Task Template");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-t/--test-val", "Test value", &test_val, ArgParse::Argument::Optional, &test_passed);
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

	std::vector<std::vector<int>> data;

	std::string line;
	while(std::getline(infile, line)) {
		std::vector<int> line_values;
		std::istringstream iss(line);
        int val;
        while(iss >> val) {
            line_values.push_back(val);
        }
		data.push_back(line_values);
	}

	if(verbose) {
		for(size_t line_idx = 0; line_idx < data.size(); ++line_idx) {
			for(size_t idx = 0; idx < data[line_idx].size(); ++idx) {
				std::cout << data[line_idx][idx] << " ";
			}
			std::cout << std::endl;
		}
	}

	int difference_sum = 0;
	for(size_t line_idx = 0; line_idx < data.size(); ++line_idx) {
		int min_val = std::numeric_limits<int>::max();
		int max_val = std::numeric_limits<int>::min();
		for(size_t idx = 0; idx < data[line_idx].size(); ++idx) {
			int val = data[line_idx][idx];
			if(val < min_val) {
				min_val = val;
			}
			if(val > max_val) {
				max_val = val;
			}
		}
		difference_sum += (max_val-min_val);
	}

	std::cout << "Diff checksum " << difference_sum << std::endl;
	if(test_passed) {
		if(difference_sum == test_val) {
			std::cout << "Test Passed!" << std::endl;
		} else {
			std::cout << "Test Failed!" << std::endl;
		}
	}

	return 0;
}
