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

typedef unsigned char type;

void hash_round(std::vector<type>& list, const std::vector<type>& lengths, size_t& skip, size_t size) {
    // Perform 
    size_t it = 0;
    size_t current_pos = 0;
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

        // Move current_position
        current_pos = (current_pos+len+skip)%size;
        skip += 1;
        it += 1;
    }

}

std::string knot_hash(std::vector<type>& list, const std::vector<type>& input_lengths, size_t size) {
    size_t skip = 0;
    size_t n_it = 0;
    while(n_it < 64) {
        hash_round(list, input_lengths, skip, size);
    }
    std::vector<type> dense_hash;
    for(size_t i = 0; i < 16; ++i) {
        type val = list[i*16];
        for(size_t j = 1; j < 16; ++j) {
            val = val^list[i*16+j];
        }
        dense_hash.push_back(val);
    }
    std::stringstream ss;
    for(type& v: dense_hash) {
        ss << std::hex << v;
    }
    return ss.str();
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    size_t size = 256;
    std::string test_string;
    bool test_string_given = false;
	ArgParse::ArgParser Parser("Day 10");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-s/--size", "Size of the list of numbers", &size);
    Parser.AddArgument("-str/--test-string", "string to test for full hash", &test_string, ArgParse::Argument::Optional, &test_string_given);

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
    std::vector<type> input_lengths;
    if(test_string_given) {
        for(size_t i = 0; i < line.size(); ++i) {
            input_lengths.push_back(test_string[i]);
        }
        lengths.push_back(17);
        lengths.push_back(31);
        lengths.push_back(73);
        lengths.push_back(47);
        lengths.push_back(23);
    } else {
        for(size_t i = 0; i < line.size(); ++i) {
            input_lengths.push_back(line[i]);
        }
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
            std::cout << (size_t) v << std::endl;
        }
        std::cout << "chars received" << std::endl;
        for(size_t i = 0; i < input_lengths.size(); ++i) {
            std::cout << ((int)input_lengths[i]) << std::endl;
        }
    }

    if(verbose) {
        std::cout << "initial number list" << std::endl;
        for(auto& v: list) {
            std::cout << (size_t) v << std::endl;
        }
    }

    std::vector<type> list_copy = list;
    
    size_t skip = 0;
    hash_round(list_copy, lengths, skip, size);

    std::cout << "Task 1: product: " << ((size_t)list_copy[0])*((size_t)list_copy[1]) << std::endl;

    // Add the special numbers to the lengths
    lengths.push_back(17);
    lengths.push_back(31);
    lengths.push_back(73);
    lengths.push_back(47);
    lengths.push_back(23);

    if(verbose) {
        std::cout << "Part 2 input lengths: " << std::endl;
        for(auto& v: lengths) {
            std::cout << ((int) v) << std::endl;
        }
    }

    std::string hash = knot_hash(list, lengths, size);

    std::cout << "Task 2: hash: " << hash << std::endl;

	return 0;
}
