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

    std::vector<type> banks;
    std::string line;
    if(!std::getline(infile, line)) {
        std::cerr << "There was a problem reading the input!" << std::endl;
        return -1;
    }

    std::istringstream iss(line);
    type bank_val = 0;
    while(iss >> bank_val) {
        banks.push_back(bank_val);
    }

    type num_banks = (type) banks.size();

    if(verbose) {
        std::cout << "received banks: " << std::endl;
        for(const auto& i: banks) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    std::vector<std::vector<type>> encountered_banks;

    if(verbose) {
        std::cout << "We now perform the redistributions" << std::endl;
    }
    int num = 0;
    while(!hasElement(encountered_banks, banks)) {
        // Add this new bank to the encountered_bank
        encountered_banks.push_back(banks);
        // Count number of redistributions
        if(verbose) {
            std::cout << num << ": ";
            for(const auto& i: banks) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
        // Determine first idx for a max element
        size_t max_idx = 0;
        for(size_t idx = 0; idx < banks.size(); ++idx) {
            if (banks[idx] > banks[max_idx]) {
                max_idx = idx;
            }
        }
        int redist_amount = banks[max_idx];
        banks[max_idx] = 0;
        int for_all = redist_amount/num_banks;
        int left_over = redist_amount%num_banks;
        for(int i = 1; i <= num_banks; ++i) {
            int new_idx = (max_idx+i)%num_banks;
            banks[new_idx] += for_all;
            if(i <= left_over) {
                banks[new_idx] += 1;
            }
        }
        // Increment counter.
        num += 1;
    }

    std::cout << "Task 1: There were " << num << " redistributions before a repeat!" << std::endl;

    int initial = 0;
    for(; initial < num; ++initial) {
        if(encountered_banks[initial] == banks) {
            break;
        }
    }

    std::cout << "Task 2: The loop length was: " << num-initial << std::endl;

	return 0;
}
