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

type severity(type t_dep, type max, const std::unordered_map<type,type>& sec_layers) {
    type answer = 0;
	type s_position = t_dep;
	type p_pos = -1;
    while(p_pos <= max) {
		// Iterate packet position
		p_pos += 1;
		// Test for detection by the security
        bool layer_exists = false;
        for(const auto& p: sec_layers) {
            if(p.first == p_pos) {
                layer_exists = true;
                break;
            }
        }
        if(layer_exists) {
            type range = sec_layers.at(p_pos);
            type sec_position = s_position%(2*(range-1));
            if(sec_position >= range) {
                sec_position = (2*(range-1))-sec_position;
            }
			std::cout << "p_pos: " << p_pos << " s_position: " << s_position << " sec_position: " << sec_position << std::endl;
            if(sec_position == 0) {
                answer += p_pos*range;
            }
        }
		// Iterate security position
		s_position += 1;
    }
    return answer;
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

    //type sev = severity(0, max, sec_layers);

    //std::cout << "Task 1: The severity of leaving at t=0: " << sev << std::endl;

	//type delay = 0;
	//std::cout << "delay " << delay << std::endl;
	//while(severity(delay, max, sec_layers) != 0) {
	//	delay += 1;
	//	std::cout << "delay " << delay << std::endl;
	//}

	std::cout << severity(10, max, sec_layers) << std::endl;
	//std::cout << severity(4, max, sec_layers) << std::endl;
	//std::cout << "Smallest delay to not be caught: " << delay << std::endl;

	return 0;
}
