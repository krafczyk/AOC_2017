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

class operation {
    public:
        operation() {}
        virtual ~operation() {}
        virtual void operator()(std::string& input) = 0;
};

class swap: public operation {
    public:
        swap(size_t i) {
            this->i = i;
        }
        void operator()(std::string& input) {
            std::string part_1 = input.substr(input.size()-i);
            std::string part_2 = input.substr(0,input.size()-i);
            input = part_1+part_2;
        }
    private:
        size_t i;
};

class exchange: public operation {
    public:
        exchange(size_t i, size_t j) {
            this->i = i;
            this->j = j;
        }
        void operator()(std::string& input) {
            char temp = input[i];
            input[i] = input[j];
            input[j] = temp;
        }
    private:
        size_t i;
        size_t j;
};

class partner: public operation {
    public:
        partner(char ci, char cj) {
            this->ci = ci;
            this->cj = cj;
        }
        void operator()(std::string& input) {
            size_t i = std::numeric_limits<size_t>::max();
            size_t j = std::numeric_limits<size_t>::max();
            for(size_t idx = 0; idx < input.size(); ++idx) {
                if((i != std::numeric_limits<size_t>::max())&&(j != std::numeric_limits<size_t>::max())) {
                    break;
                }
                if(input[idx] == ci) {
                    i = idx;
                }
                if(input[idx] == cj) {
                    j = idx;
                }
            }
            char temp = input[j];
            input[j] = input[i];
            input[i] = temp;
        }
    private:
        char ci;
        char cj;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    size_t s = 16;
	ArgParse::ArgParser Parser("Day 16");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("--size", "Number of programs", &s, ArgParse::Argument::Optional);

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
    if(!std::getline(infile, line)) {
        std::cerr << "Problem reading input file" << std::endl;
        return -1;
    }

    std::string programs;
    for(size_t idx = 0; idx < s; ++idx) {
        programs.push_back('a'+idx);
    }

    if(verbose) {
        std::cout << "Starting with: " << programs << std::endl;
    }

    std::vector<operation*> dance;

    std::vector<std::string> values;
    values.push_back(programs);

    std::regex op_match("(s[0-9]+|x[0-9]+/[0-9]+|p[a-p]/[a-p])");
    std::regex spin_match("s([0-9]+)");
    std::regex exchange_match("x([0-9]+)/([0-9]+)");
    std::regex partner_match("p([a-p])/([a-p])");
    std::sregex_iterator beg(line.begin(), line.end(), op_match);
    std::sregex_iterator end;
    for(std::sregex_iterator it = beg; it != end; ++it) {
        std::string match = it->str();
        if(match[0] == 's') {
            std::smatch matcher;
            if(!std::regex_match(match, matcher, spin_match)) {
                std::cerr << "Problem with the spin matcher" << std::endl;
                return -3;
            }
            size_t i = fetch_value<size_t>(matcher[1].str());
            dance.push_back(new swap(i));

        } else if (match[0] == 'x') {
            std::smatch matcher;
            if(!std::regex_match(match, matcher, exchange_match)) {
                std::cerr << "Problem with the exchange matcher" << std::endl;
                return -4;
            }
            size_t i = fetch_value<size_t>(matcher[1].str());
            size_t j = fetch_value<size_t>(matcher[2].str());
            dance.push_back(new exchange(i,j));

        } else if (match[0] == 'p') {
            std::smatch matcher;
            if(!std::regex_match(match, matcher, partner_match)) {
                std::cerr << "Problem with the partner matcher" << std::endl;
                return -5;
            }
            char ci = matcher[1].str()[0];
            char cj = matcher[2].str()[0];
            dance.push_back(new partner(ci,cj));

        } else {
            std::cerr << "This shouldn't happen!: " << match[0] << std::endl;
            return -2;
        }
    }

    size_t repeat = 0;
    while(true) {
        // Do the dance
        for(size_t idx = 0; idx < dance.size(); ++idx) {
            (*dance[idx])(programs);
        }
        if(hasElement(values, programs)) {
            // Find where the repeat starts
            for(size_t i = 0; i < values.size(); ++i) {
                if(values[i] == programs) {
                    repeat = i;
                }
            }
            break;
        } else {
            values.push_back(programs);
        }

    }

    std::cout << "Task 1: First dance result: " << values[1] << std::endl;
    size_t period = values.size()-repeat;
    std::cout << "Task 2: End of dance: " << values[(1000000000-repeat)%period] << std::endl;

    for(size_t idx = 0; idx < dance.size(); ++idx) {
        delete dance[idx];
    }

	return 0;
}
