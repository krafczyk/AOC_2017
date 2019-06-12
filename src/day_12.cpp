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

typedef int type;

class node {
    public:
        std::vector<type> links;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 12");
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

    std::unordered_map<type,node*> node_map;

    std::string line;
    std::regex num_match("([0-9]+)");
    while(std::getline(infile, line)) {
        auto line_begin = line.begin();
        auto line_end = line.end();
        auto regit_begin = std::sregex_iterator(line_begin, line_end, num_match);
        auto regit_end = std::sregex_iterator();
        std::vector<type> ids;
        for(auto regit = regit_begin; regit != regit_end; ++regit) {
            ids.push_back(fetch_value<type>(regit->str()));
        }
        node_map[ids[0]] = new node;
        for(size_t idx = 1; idx < ids.size(); ++idx) {
            node_map[ids[0]]->links.push_back(ids[idx]);
        }
    }

    std::vector<type> edge_elements;
    std::vector<type> visited_elements;

    // Seed with ID 0
    edge_elements.push_back(0);

    while(edge_elements.size() != 0) {
        // get the latest edge element to explore
        type current = edge_elements.back();
        edge_elements.pop_back();
        // Add current to the visited elements
        visited_elements.push_back(current);
        // Add links to edge_elements if not in
        // either of those groups
        for(type id: node_map[current]->links) {
            if((!hasElement(edge_elements,id))&&(!hasElement(visited_elements,id))) {
                edge_elements.push_back(id);
            }
        }
    }

    std::cout << "Task 1: There are ";
    std::cout << visited_elements.size();
    std::cout << " which can communicate with ID 0." << std::endl;

	return 0;
}
