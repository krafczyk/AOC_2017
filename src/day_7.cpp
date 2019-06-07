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
            this->tower_weight = 0;
            this->child_names = child_names;
        }
        friend std::ostream& operator<<(std::ostream& out, const node& rhs);
        std::string name;
        int weight;
        int tower_weight;
        std::vector<std::string> child_names;
};

std::ostream& operator<<(std::ostream& out, const node& rhs) {
    out << rhs.name << " (" << rhs.weight << ")[" << rhs.tower_weight << "]";
    if(rhs.child_names.size() != 0) {
        out << " -> ";
        bool first = true;
        for(auto& name: rhs.child_names) {
            if(first) {
                first = false;
            } else {
                out << ", ";
            }
            out << name;
        }
    }
    return out;
}

std::vector<node*> node_list;
std::map<std::string,node*> node_map;

int fill_tower_weight(const std::string& name) {
    int total = 0;
    for(auto& child_name: node_map[name]->child_names) {
        fill_tower_weight(child_name);
        total += node_map[child_name]->tower_weight;
    }
    total += node_map[name]->weight;
    node_map[name]->tower_weight = total;
    return total;
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

    std::map<std::string,int> num_parents;
    for(auto node: node_list) {
        num_parents[node->name] = 0;
    }
    for(auto node: node_list) {
        for(auto node_2: node_list) {
            if(node_2 == node) {
                // Skip case where its the same node.
                continue;
            }
            if(hasElement(node_2->child_names, node->name)) {
                num_parents[node->name] += 1;
            }
        }
    }
    std::string root_node;
    for(auto& p: num_parents) {
        if(p.second == 0) {
            if(root_node != "") {
                std::cerr << "There are more than one root node candidates!!" << std::endl;
            } else {
                root_node = p.first;
            }
        }
    }

    std::cout << "Task 1: the bottom program is: " << root_node << std::endl;

    fill_tower_weight(root_node);

    if(verbose) {
        std::cout << "Node list: " << std::endl;
        for(auto node: node_list) {
            std::cout << *node << std::endl;
        }
    }

    int new_weight = 0;
    int target_weight = 0;
    std::string current_node = root_node;
    std::string prev_node;
    while(true) {
        // Check that each child tower has the same weight
        std::map<int,int> child_tower_weights;
        for(auto& child_name: node_map[current_node]->child_names) {
            int child_tower_weight = node_map[child_name]->tower_weight;
            child_tower_weights[child_tower_weight] += 1;
        }
        if(child_tower_weights.size() != 1) {
            // Unbalanced -> Save target tower weight, and decend to node which disagrees.
            int num_children = 0;
            for(auto& w_p: child_tower_weights) {
                if(w_p.second > num_children) {
                    target_weight = w_p.first;
                    num_children = w_p.second;
                }
            }
            for(auto& child_name: node_map[current_node]->child_names) {
                if(node_map[child_name]->tower_weight != target_weight) {
                    current_node = child_name;
                    break;
                }
            }
        } else {
            if(target_weight == 0) {
                std::cerr << "This shouldn't happen!!" << std::endl;
                return -1;
            }
            // This program is balanced, but the previous wasn't!
            auto it = child_tower_weights.cbegin();
            new_weight = target_weight - it->first*it->second;
            break;
        }
    }

    std::cout << "Task 2: The new weight of the node should be: " << new_weight << std::endl;

	return 0;
}
