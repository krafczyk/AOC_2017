#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include "ArgParseStandalone.h"
#include "utilities.h"

class node {
    public:
        node(const std::string& name, int weight, const std::vector<std::string> child_names) {
            this->name = name;
            this->weight = weight;
            this->child_names = child_names;
        }
        std::string name;
        int weight;
        std::vector<std::string> child_names;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 7");
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

    std::vector<node*> node_list;
    std::map<std::string,node*> node_map;

    std::regex node_regex("^([a-z]*) \\(([0-9]*)\\)(?: -> (.*))?$");
    std::regex name_regex("([a-z]+)");
    std::string line;
    while(std::getline(infile, line)) {
        std::smatch match_result;
        if(!std::regex_match(line, match_result, node_regex)) {
            std::cerr << "We couldn't match a line!" << std::endl;
            return -1;
        }
        std::string name = match_result[1].str();
        int weight = fetch_value<int>(match_result[2].str());
        std::vector<std::string> child_names;
        if(match_result.size() == 4) {
            // We have children.
            std::string child_list = match_result[3].str();
            auto child_names_begin = std::sregex_iterator(child_list.begin(), child_list.end(), name_regex);
            auto child_names_end = std::sregex_iterator();

            for(std::sregex_iterator i = child_names_begin; i != child_names_end; ++i) {
                std::smatch match = *i;
                child_names.push_back(match.str());
            }
        }
        node* new_node = new node(name, weight, child_names);
        node_list.push_back(new_node);
        node_map[name] = new_node;
    }

	return 0;
}
