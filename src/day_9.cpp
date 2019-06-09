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

class node {
    public:
        std::vector<node*> children;
};

void destroy_tree(node* root) {
    for(node* n: root->children) {
        destroy_tree(n);
    }
    delete root;
    return;
}

int tree_score(node* root) {
    std::map<node*,int> node_depths;
    std::vector<node*> edge_nodes;
    node_depths[root] = 1;
    edge_nodes.push_back(root);
    while(edge_nodes.size() != 0) {
        node* tn = edge_nodes.back();
        edge_nodes.pop_back();
        for(node* child: tn->children) {
            node_depths[child] = node_depths[tn]+1;
            edge_nodes.push_back(child);
        }
    }
    int score = 0;
    for(auto& p: node_depths) {
        score += p.second;
    }
    return score;
}

int build_tree(node* parent, const std::string& input, size_t idx) {
    bool ignore_next_char = false;
    bool in_garbage = false;
    while(idx < input.size()) {
        // Handle exclamation marks.
        if(ignore_next_char) {
            ignore_next_char = false;
            idx++;
            continue;
        }
        // The current character.
        char c = input[idx];
        // Handle cancellation of characters
        if(c == '!') {
            ignore_next_char = true;
            idx++;
            continue;
        }
        // Handle exit of garbage section
        if(in_garbage) {
            if(c != '>') {
                idx++;
                continue;
            } else {
                idx++;
                in_garbage = false;
                continue;
            }
        }
        // Handle start of garbage
        if(c == '<') {
            in_garbage = true;
            idx++;
            continue;
        }
        // Handle opening of a new group
        if(c == '{') {
            node* new_child = new node;
            parent->children.push_back(new_child);
            idx++;
            idx = build_tree(new_child, input, idx);
            continue;
        }
        // Handle closing of the current group
        if(c == '}') {
            idx++;
            break;
        }
        if(c == ',') {
            idx++;
            continue;
        }
        std::cerr << "We should never get to this point!!!" << std::endl;
        throw;
    }
    return idx;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 9");
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

    std::string input_string;
    if(!std::getline(infile, input_string)) {
        std::cerr << "Can't read from input file." << std::endl;
        return -1;
    }

    // Create the root node.
    node* root = new node;

    size_t idx = 0;
    // Check that the first character is the start of a group
    if(input_string[idx] != '{') {
        std::cerr << "Input data is formatted incorrectly!" << std::endl;
        return -1;
    }
    idx += 1;

    // Build up the tree
    build_tree(root, input_string, idx);

    // Compute tree score
    std::cout << "Task 1: the total tree score is: " << tree_score(root) << std::endl;

    // Clean everything up
    destroy_tree(root);
	return 0;
}
