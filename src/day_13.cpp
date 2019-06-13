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

class sec_layer {
    public:
        sec_layer(int depth) {
            this->depth = depth;
        }
    private:
        int depth;
};

typedef int type;

bool severity(type t_dep, type max, const std::unordered_map<type,type>& sec_layers, type& sev) {
    sev = 0;
	bool detected = false;
    for(type l = 0; l <= max; ++l) {
		type t = l+t_dep;
		// Test for detection by the security
        bool layer_exists = false;
        for(const auto& p: sec_layers) {
            if(p.first == l) {
                layer_exists = true;
                break;
            }
        }
        if(layer_exists) {
            type range = sec_layers.at(l);
            type sec_position = t%(2*(range-1));
            if(sec_position >= range) {
                sec_position = (2*(range-1))-sec_position;
            }
            if(sec_position == 0) {
                sev += l*range;
				detected = true;
            }
        }
    }
    return detected;
}

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

    std::unordered_map<type,type> sec_layers;

    std::string line;
    std::regex matcher("^([0-9]+): ([0-9]+)$");
    type max = std::numeric_limits<type>::min();
    while(std::getline(infile, line)) {
        std::smatch the_match;
        if(!std::regex_match(line, the_match, matcher)) {
            std::cerr << "There was a problem matching a line!" << std::endl;
            return -1;
        }
        type layer = fetch_value<type>(the_match[1].str());
        type depth = fetch_value<type>(the_match[2].str());
        if(layer > max) {
            max = layer;
        }
        sec_layers[layer] = depth;
    }

    type sev;
	severity(0, max, sec_layers, sev);

    std::cout << "Task 1: The severity of leaving at t=0: " << sev << std::endl;

	type delay = 0;
	while(severity(delay, max, sec_layers, sev) != 0) {
		delay += 1;
	}

	std::cout << "Task 2: Smallest delay to not be caught: " << delay << std::endl;

	return 0;
}
