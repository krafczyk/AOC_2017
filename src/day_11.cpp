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

class point {
    public:
        point() {
            x = 0;
            y = 0;
        }
        point(const std::pair<type,type>& rhs) {
            x = rhs.first;
            y = rhs.second;
        }
        point(type x, type y) {
            this->x = x;
            this->y = y;
        }
        point(const point& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
        }
        point operator+(const point& rhs) const {
            return point(x+rhs.x,y+rhs.y);
        }
        point operator-(const point& rhs) const {
            return point(x-rhs.x,y-rhs.y);
        }
        point operator*(type n) const {
            return point(x*n,y*n);
        }
        type x;
        type y;
        friend std::ostream& operator<<(std::ostream& out, const point& p);
};

std::ostream& operator<<(std::ostream& out, const point& p) {
    out << p.x << "," << p.y;
    return out;
}

std::unordered_map<std::string,point> dirs {
    {"n", point(0,2)},
    {"s", point(0,-2)},
    {"ne", point(1,1)},
    {"sw", point(-1,-1)},
    {"nw", point(-1,1)},
    {"se", point(1,-1)}
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

    std::string line;
    if(!std::getline(infile, line)) {
        std::cerr << "There was a problem getting problem input!" << std::endl;
        return -1;
    }

    point current(0,0);

    std::regex dir_match("([nesw]+)");
    auto line_beg = std::sregex_iterator(line.begin(), line.end(), dir_match);
    auto line_end = std::sregex_iterator();
    for(std::sregex_iterator i = line_beg; i != line_end; ++i) {
        current = current+dirs[i->str()];
    }

    if(verbose) {
        std::cout << "We wind up at: " << current << std::endl;
    }

	return 0;
}
