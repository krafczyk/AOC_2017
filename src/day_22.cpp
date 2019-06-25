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

typedef int type_t;

class point: public point2d<type_t> {
    public:
        point(type_t x = 0, type_t y = 0) : point2d<type_t>(x,y) {}
        point(const point& rhs) : point2d<type_t>(rhs) {};
        point& operator=(const point& rhs) {
            point2d<type_t>::operator=(rhs);
            return *this;
        }
        point operator+(const point& rhs) {
            point New(*this);
            New.x += rhs.x;
            New.y += rhs.y;
            return New;
        }
        bool operator<(const point& rhs) const {
            if(this->x < rhs.x) {
                return true;
            } else {
                if(this->y < rhs.y) {
                    return true;
                }
            }
            return false;
        }
        bool operator>(const point& rhs) const {
            if(this->x > rhs.x) {
                return true;
            } else {
                if(this->y > rhs.y) {
                    return true;
                }
            }
            return false;
        }
        friend std::ostream& operator<<(std::ostream& out, const point& p) {
            out << p.x << "," << p.y;
            return out;
        }
};

point dirs[4] = {
    {1,0},
    {0,1},
    {-1,0},
    {0,-1}
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 22");
    int num_bursts = 0;
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-n/--num", "Number of bursts of activity to simulate", &num_bursts);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::vector<std::string> input_data;
    std::string line;
    while(std::getline(infile, line)) {
        input_data.push_back(line);
    }

    // Initialize infected map
    std::set<point> infected;
    type_t height = input_data.size();
    type_t width = input_data[0].size();
    for(int row_i = 0; row_i < height; ++row_i) {
        for(int col_i = 0; col_i < width; ++col_i) {
            if(input_data[row_i][col_i] == '#') {
                infected.insert(point(col_i-(width/2),(height/2)-row_i));
            }
        }
    }

    if(verbose) {
        std::cout << "Initial infected points" << std::endl;
        for(auto& p: infected) {
            std::cout << p << std::endl;
        }
    }

    point current_position(0,0);
    int current_dir = 1;
    auto turn_left = [&current_dir]() {
        current_dir = (current_dir+1)%4;
    };
    auto turn_right = [&current_dir]() {
        current_dir -= 1;
        if(current_dir < 0) {
            current_dir += 4;
        }
        current_dir = current_dir%4;
    };

    int num = 0;
    int num_infected = 0;
    while(num < num_bursts) {
        if(verbose) {
            std::cout << "Current " << current_position << " " << dirs[current_dir] << std::endl;
        }
        auto pos_it = std::find(infected.begin(), infected.end(), current_position);
        if(pos_it == infected.end()) {
            if(verbose) {
                std::cout << "Infecting" << std::endl;
            }
            // Not infected.
            num_infected += 1;
            infected.insert(current_position);
            turn_left();
        } else {
            if(verbose) {
                std::cout << "Disinfecting" << std::endl;
            }
            // infected
            infected.erase(pos_it);
            turn_right();
        }
        if(verbose) {
            std::cout << "new state" << std::endl;
            for(auto& p: infected) {
                std::cout << p << std::endl;
            }
        }
        // advance position
        current_position = current_position+dirs[current_dir];
        num += 1;
    }

    if(verbose) {
        std::cout << "Final state" << std::endl;
        for(auto& p: infected) {
            std::cout << p << std::endl;
        }
    }

    std::cout << num_infected << " computers got infected" << std::endl;

	return 0;
}
