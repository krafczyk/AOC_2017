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
    std::vector<type> node_ids;

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
        node_ids.push_back(ids[0]);
        for(size_t idx = 1; idx < ids.size(); ++idx) {
            node_map[ids[0]]->links.push_back(ids[idx]);
        }
    }

    std::vector<type> edge_elements;
    std::vector<type> visited_elements;
    std::vector<std::vector<type>> groups;

    auto total_visited = [&groups]() {
        size_t total = 0;
        for(const auto& group: groups) {
            total += group.size();
        }
        return total;
    };

    while(total_visited() < node_map.size()) {
        // Start with a seed element 
        type initial_id = node_ids.back();
        node_ids.pop_back();
        edge_elements.push_back(initial_id);

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
                    // Remove from the overall node id list.
                    node_ids.erase(std::find(node_ids.begin(), node_ids.end(), id));
                }
            }
        }

        // Add the new group
        groups.push_back(visited_elements);
        // Clear the old variables.
        visited_elements.clear();
    }

    auto zero_group_it = std::find_if(groups.cbegin(), groups.cend(), [](const std::vector<type>& group) {
        if(hasElement(group,0)) {
            return true;
        } else {
            return false;
        }
    });

    std::cout << "Task 1: There are ";
    std::cout << zero_group_it->size();
    std::cout << " which can communicate with ID 0." << std::endl;

    std::cout << "Task 2: There are " << groups.size() << " total groups." << std::endl;

	return 0;
}
