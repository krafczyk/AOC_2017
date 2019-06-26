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

typedef int type_t;
typedef point2d<type_t> point;

std::ostream& operator<<(std::ostream& out, const point& p) {
    out << p.x << "/" << p.y;
    return out;
}

bool valid_component(type_t v, const point& p) {
    if((p.x == v)||(p.y == v)) {
        return true;
    } else {
        return false;
    }
}

type_t component_strength(const point& p) {
    return p.x+p.y;
}

type_t other_component(type_t v, const point& p) {
    if(p.x == v) {
        return p.y;
    } else {
        return p.x;
    }
}

type_t strongest_bridge(type_t v, std::vector<point>& components) {
    type_t strength = 0;
    for(size_t i = 0; i < components.size(); ++i) {
        if(valid_component(v, components[i])) {
            std::vector<point> temp = components;
            temp.erase(temp.begin()+i);
            strength = std::max(strength, component_strength(components[i])+strongest_bridge(other_component(v, components[i]),temp));
        }
    }
    return strength;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 24");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    std::vector<point> components;

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string line;
    std::regex component_pattern("([0-9]+)/([0-9]+)");
    while(std::getline(infile, line)) {
        std::smatch match;
        if(!std::regex_match(line, match, component_pattern)) {
            std::cerr << "Error matching line" << std::endl;
            throw;
        }
        components.push_back(point(fetch_value<type_t>(match[1]), fetch_value<type_t>(match[2])));
    }

    if(verbose) {
        std::cout << "We found the following components: " << std::endl;
        for(const point& p: components) {
            std::cout << p << std::endl;
        }
    }

    std::cout << "Task 1: strongest bridge has strength: " << strongest_bridge(0, components) << std::endl;

	return 0;
}
