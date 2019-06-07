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
	ArgParse::ArgParser Parser("Day 4");
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

    std::vector<std::vector<std::string>> passphrases;
    std::string line;
    while(std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string word;
        std::vector<std::string> passphrase;
        while(iss >> word) {
            passphrase.push_back(word);
            word.clear();
        }
        passphrases.push_back(passphrase);
    }

    if(verbose) {
        std::cout << "The passphrases are:" << std::endl;
        for(const auto& v: passphrases) {
            for(const auto& word: v) {
                std::cout << word << " ";
            }
            std::cout << std::endl;
        }
    }

    int num_valid = 0;
    for(const auto& passphrase: passphrases) {
        bool is_valid = true;
        for(auto word_it = passphrase.cbegin(); word_it != passphrase.cend()-1; ++word_it) {
            for(auto word_it_2 = word_it+1; word_it_2 != passphrase.cend(); ++word_it_2) {
                if(*word_it == *word_it_2) {
                    is_valid = false;
                    break;
                }
            }
            if(!is_valid) {
                break;
            }
        }
        if(is_valid) {
            num_valid += 1;
        }
    }

    std::cout << "There are " << num_valid << " valid passphrases" << std::endl;

	return 0;
}
