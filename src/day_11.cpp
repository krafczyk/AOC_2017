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
        type dist(const point& p) const {
            float dx = this->x - p.x;
            float dy = this->y - p.y;
            type n1 = 0;
            type n2 = 0;
            type min_dist = std::numeric_limits<type>::max();
            auto is_integral = [](float f) {
                float integral = 0;
                float fractional = std::modf(f, &integral);
                if(fractional == 0.) {
                    return true;
                } else {
                    return false;
                }
            };
            auto check_and_update = [&](float n1p, float n2p) {
                // Check if they're fractional
                if(is_integral(n1p)&&is_integral(n2p)) {
                    type td = std::abs(n1p)+std::abs(n2p);
                    if(td < min_dist) {
                        min_dist = td;
                        n1 = (type)n1p;
                        n2 = (type)n2p;
                    }
                }
            };
            float n11 = dx;
            float n12 = -dx/2+dy/2;
            check_and_update(n11, n12);
            float n21 = dx;
            float n22 = dx/2+dy/2;
            check_and_update(n21, n22);
            float n31 = dx/2+dy/2;
            float n32 = dx/2-dy/2;
            check_and_update(n31, n32);
            return min_dist;
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
    point origin(0,0);
    type max_dist = 0;

    std::regex dir_match("([nesw]+)");
    auto line_beg = std::sregex_iterator(line.begin(), line.end(), dir_match);
    auto line_end = std::sregex_iterator();
    for(std::sregex_iterator i = line_beg; i != line_end; ++i) {
        current = current+dirs[i->str()];
        type d = current.dist(origin);
        if(d > max_dist) {
            max_dist = d;
        }
    }

    if(verbose) {
        std::cout << "We wind up at: " << current << std::endl;
    }

    std::cout << "Task 1: Distance to the origin: " << current.dist(point(0,0)) << std::endl;
    std::cout << "Task 2: Max distance from origin: " << max_dist << std::endl;

	return 0;
}
