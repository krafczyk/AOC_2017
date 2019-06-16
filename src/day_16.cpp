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

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    size_t s = 16;
	ArgParse::ArgParser Parser("Day 16");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("--size", "Number of programs", &s, ArgParse::Argument::Optional);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string line;
    if(!std::getline(infile, line)) {
        std::cerr << "Problem reading input file" << std::endl;
        return -1;
    }

    std::string programs;
    for(size_t idx = 0; idx < s; ++idx) {
        programs.push_back('a'+idx);
    }

    if(verbose) {
        std::cout << "Starting with: " << programs << std::endl;
    }

    std::regex op_match("(s[0-9]+|x[0-9]+/[0-9]+|p[a-p]/[a-p])");
    std::regex spin_match("s([0-9]+)");
    std::regex exchange_match("x([0-9]+)/([0-9]+)");
    std::regex partner_match("p([a-p])/([a-p])");
    std::sregex_iterator beg(line.begin(), line.end(), op_match);
    std::sregex_iterator end;
    for(std::sregex_iterator it = beg; it != end; ++it) {
        std::string match = it->str();
        if(match[0] == 's') {
            std::smatch matcher;
            if(!std::regex_match(match, matcher, spin_match)) {
                std::cerr << "Problem with the spin matcher" << std::endl;
                return -3;
            }
            size_t i = fetch_value<size_t>(matcher[1].str());
            std::string part_1 = programs.substr(programs.size()-i);
            std::string part_2 = programs.substr(0,programs.size()-i);
            programs = part_1+part_2;
        } else if (match[0] == 'x') {
            std::smatch matcher;
            if(!std::regex_match(match, matcher, exchange_match)) {
                std::cerr << "Problem with the exchange matcher" << std::endl;
                return -4;
            }
            size_t i = fetch_value<size_t>(matcher[1].str());
            size_t j = fetch_value<size_t>(matcher[2].str());
            char temp = programs[i];
            programs[i] = programs[j];
            programs[j] = temp;
        } else if (match[0] == 'p') {
            std::smatch matcher;
            if(!std::regex_match(match, matcher, partner_match)) {
                std::cerr << "Problem with the partner matcher" << std::endl;
                return -5;
            }
            char ci = matcher[1].str()[0];
            char cj = matcher[2].str()[0];
            size_t i = std::numeric_limits<size_t>::max();
            size_t j = std::numeric_limits<size_t>::max();
            for(size_t idx = 0; idx < programs.size(); ++idx) {
                if((i != std::numeric_limits<size_t>::max())&&(j != std::numeric_limits<size_t>::max())) {
                    break;
                }
                if(programs[idx] == ci) {
                    i = idx;
                }
                if(programs[idx] == cj) {
                    j = idx;
                }
            }
            char temp = programs[j];
            programs[j] = programs[i];
            programs[i] = temp;
        } else {
            std::cerr << "This shouldn't happen!: " << match[0] << std::endl;
            return -2;
        }
    }

    std::cout << "Task 1: The final order is: " << programs << std::endl;

	return 0;
}
