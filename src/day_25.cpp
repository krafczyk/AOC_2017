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

typedef long type_t;
typedef std::unordered_map<type_t,int> tape_t;

tape_t tape;
type_t left = -1;
type_t right = 1;

std::map<type_t,std::string> dir_map = { {-1,"left"}, {1,"right"}};

class state {
    public:
        state(int v0, type_t dir0, type_t state0, int v1, type_t dir1, type_t state1) {
            this->v0 = v0;
            this->dir0 = dir0;
            this->state0 = state0;
            this->v1 = v1;
            this->dir1 = dir1;
            this->state1 = state1;
        }
        void operator()(type_t& cur_position, type_t& cur_state, tape_t& tape) {
            if(tape[cur_position] == 0) {
                tape[cur_position] = v0;
                cur_position += dir0;
                cur_state = state0;
            } else {
                tape[cur_position] = v1;
                cur_position += dir1;
                cur_state = state1;
            }
        }
        friend std::ostream& operator<<(std::ostream& out, const state& st) {
            out << "If the current value is 0:" << std::endl;
            out << "  - Write the value " << st.v0 << "." << std::endl;
            out << "  - Move one slot to the " << dir_map[st.dir0] << "." << std::endl;
            out << "  - Continue with state " << (char)('A'+st.state0) << "." << std::endl;
            out << "If the current value is 1:" << std::endl;
            out << "  - Write the value " << st.v1 << "." << std::endl;
            out << "  - Move one slot to the " << dir_map[st.dir1] << "." << std::endl;
            out << "  - Continue with state " << (char)('A'+st.state1) << "." << std::endl;
            return out;
        }
    private:
        int v0;
        type_t dir0;
        type_t state0;
        int v1;
        type_t dir1;
        type_t state1;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 25");
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
    std::string line;

    auto get_line = [&]() {
        if(!std::getline(infile,line)) {
            std::cerr << "There was an error getting a required line!" << std::endl;
            throw;
        }
    };

    auto get_value = [](const std::string line, const std::regex& r) {
        std::smatch match;
        if(!std::regex_match(line, match, r)) {
            std::cerr << "There was a problem matching a line!" << std::endl;
            std::cerr << "Line was: (" << line << ")" << std::endl;
            throw;
        }
        return match[1].str();
    };

    type_t max_state = 0;
    type_t initial_state = 0;

    auto get_state = [&max_state](const std::string& in) {
        type_t v = ((unsigned char)in[0])-((unsigned char)'A');
        max_state = std::max(max_state,v);
        return v;
    };

    std::string temp;

    // Get initial state
    get_line();
    temp = get_value(line, std::regex("^Begin in state ([A-Z]+).$"));
    initial_state = get_state(temp);

    size_t num_steps = 0;
    get_line();
    temp = get_value(line, std::regex("^Perform a diagnostic checksum after ([0-9]+) steps.$"));
    num_steps = fetch_value<size_t>(temp);

    // Build state array
    std::map<type_t,state*> state_map;

    // Get the states
    std::regex state_pattern("^In state ([A-Z]+):$");
    std::regex val_pattern("^    - Write the value ([01]).$");
    std::regex move_pattern("^    - Move one slot to the (left|right).$");
    std::regex st_pattern("^    - Continue with state ([A-Z]+).$");
    type_t st_i = 0;
    while(std::getline(infile,line)) {
        get_line();
        temp = get_value(line, state_pattern);
        type_t state_i = get_state(temp);

        int v0 = 0;
        type_t dir0 = 0;
        type_t state0 = 0;
        int v1 = 0;
        type_t dir1 = 0;
        type_t state1 = 0;

        get_line();
        get_line();
        temp = get_value(line, val_pattern);
        v0 = fetch_value<int>(temp);
        get_line();
        temp = get_value(line, move_pattern);
        if(temp == "left") {
            dir0 = left;
        } else {
            dir0 = right;
        }
        get_line();
        temp = get_value(line, st_pattern);
        state0 = get_state(temp);

        get_line();
        get_line();
        temp = get_value(line, val_pattern);
        v1 = fetch_value<int>(temp);
        get_line();
        temp = get_value(line, move_pattern);
        if(temp == "left") {
            dir1 = left;
        } else {
            dir1 = right;
        }
        get_line();
        temp = get_value(line, st_pattern);
        state1 = get_state(temp);

        state_map[state_i] = new state(v0,dir0,state0,v1,dir1,state1);
        st_i += 1;
    }

    // Build state array
    state** states = new state*[max_state+1];
    for(type_t i = 0; i <= max_state; ++i) {
        states[i] = state_map[i];
    }

    if(verbose) {
        std::cout << "We extracted the following program: " << std::endl;
        for(size_t i = 0; i < (size_t) max_state+1; ++i) {
            std::cout << "State " << i << std::endl;
            std::cout << *states[i] << std::endl;
        }
    }

    // Run program
    type_t current_state = initial_state;
    type_t current_position = 0;
    for(size_t i = 0; i < num_steps; ++i) {
        if(verbose) {
            if((i != 0)&&(i%1000000 == 0)) {
                std::cout << i << " steps run so far." << std::endl;
            }
        }
        (*states[current_state])(current_position, current_state, tape);
    }

    // Count the 1s
    size_t checksum = 0;
    for(auto& p: tape) {
        if(p.second == 1) {
            checksum += 1;
        }
    }

    std::cout << "Task 1: the checksum is: " << checksum << std::endl;

    // Cleanup after ourselves.
    for(size_t i = 0; i < (size_t) max_state+1; ++i) {
        delete states[i];
    }
    delete [] states;

	return 0;
}
