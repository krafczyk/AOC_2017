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

class conditional {
    public:
        virtual ~conditional() {}
        virtual bool operator()(type a, type b) = 0;
};

class gt : public conditional {
    public:
        bool operator()(type a, type b) {
            if (a > b) {
                return true;
            } else {
                return false;
            }
        }
};

class lt : public conditional {
    public:
        bool operator()(type a, type b) {
            if (a < b) {
                return true;
            } else {
                return false;
            }
        }
};

class eq : public conditional {
    public:
        bool operator()(type a, type b) {
            if (a == b) {
                return true;
            } else {
                return false;
            }
        }
};

class neq : public conditional {
    public:
        bool operator()(type a, type b) {
            if (a != b) {
                return true;
            } else {
                return false;
            }
        }
};

class gteq : public conditional {
    public:
        bool operator()(type a, type b) {
            if (a >= b) {
                return true;
            } else {
                return false;
            }
        }
};

class lteq : public conditional {
    public:
        bool operator()(type a, type b) {
            if (a <= b) {
                return true;
            } else {
                return false;
            }
        }
};

std::unordered_map<std::string,conditional*> conditionals = {
    {">", (conditional*) (new gt())},
    {"<", (conditional*) (new lt())},
    {"==", (conditional*) (new eq())},
    {"!=", (conditional*) (new neq())},
    {">=", (conditional*) (new gteq())},
    {"<=", (conditional*) (new lteq())},
};

class operation {
    public:
        virtual ~operation() {};
        virtual type operator()(type a, type b) = 0;
};

class increase : public operation {
    public:
        type operator()(type a, type b) {
            return a+b;
        }
};

class decrease : public operation {
    public:
        type operator()(type a, type b) {
            return a-b;
        }
};

std::unordered_map<std::string, operation*> operations = {
    {"inc", (operation*) (new increase())},
    {"dec", (operation*) (new decrease())},
};



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

    std::unordered_map<std::string,type> reg_values;

    std::regex instruction_regex("^([a-z]*) (inc|dec) ([0-9\\-]*) if ([a-z]*) ([><=!]*) ([0-9\\-]*)$", std::regex::ECMAScript);
    std::string line;
    type max_val = std::numeric_limits<type>::min();
    while(std::getline(infile, line)) {
        std::smatch instruction_match;
        if(!std::regex_match(line, instruction_match,instruction_regex)) {
            std::cerr << "Couldn't match a line!" << std::endl;
            return -1;
        }
        std::string target_reg = instruction_match[1].str();
        std::string target_op = instruction_match[2].str();
        type target_mod_val = fetch_value<type>(instruction_match[3].str());
        std::string conditional_reg = instruction_match[4].str();
        std::string conditional_op = instruction_match[5].str();
        type conditional_val = fetch_value<type>(instruction_match[6].str());
        if((*conditionals[conditional_op])(reg_values[conditional_reg],conditional_val)) {
            reg_values[target_reg] = (*operations[target_op])(reg_values[target_reg],target_mod_val);
        }
        for(auto& v: reg_values) {
            if(v.second > max_val) {
                max_val = v.second;
            }
        }
    }

    type max_val_end = std::numeric_limits<type>::min();

    for(auto& v: reg_values) {
        if(v.second > max_val_end) {
            max_val_end = v.second;
        }
    }

    std::cout << "Task 1: The largest register value is: " << max_val_end << std::endl;
    std::cout << "Task 2: The largest value stored was: " << max_val << std::endl;

    // cleanup
    for(auto& v: conditionals) {
        delete v.second;
    }
    for(auto& v: operations) {
        delete v.second;
    }

	return 0;
}
