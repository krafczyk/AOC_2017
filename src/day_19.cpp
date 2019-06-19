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

class point {
	public:
		point(int x = 0, int y = 0) {
			this->x = x;
			this->y = y;
		}
		point(const point& rhs) {
			this->x = rhs.x;
			this->y = rhs.y;
		}
		point& operator=(const point& rhs) {
			this->x = rhs.x;
			this->y = rhs.y;
			return (*this);
		}
		point operator+(const point& rhs) const {
			return point(x+rhs.x,y+rhs.y);
		}
		point operator-(const point& rhs) const {
			return point(x-rhs.x,y-rhs.y);
		}
		point operator*(int a) const {
			return point(x*a, y*a);
		}
		bool operator!=(const point& rhs) const {
			if((x==rhs.x)&&(y==rhs.y)) {
				return false;
			} else {
				return true;
			}
		}
		bool operator==(const point& rhs) const {
			if((x==rhs.x)&&(y==rhs.y)) {
				return true;
			} else {
				return false;
			}
		}
		friend std::ostream& operator<<(std::ostream& out, const point& rhs);
		int x;
		int y;
};

std::ostream& operator<<(std::ostream& out, const point& rhs) {
	out << rhs.x << "," << rhs.y;
	return out;
}

point dirs[2] = { {0,1}, {1,0} };

std::pair<std::string,int> navigate_map(array_2d<char>& map, bool verbose = false) {
	// Initialize answer
	std::string answer;
	int steps = 0;
	// Find start location
	size_t initial_x = 0;
	for(;initial_x < map.width(); ++initial_x) {
		if(map(initial_x,0) == '|') {
			break;
		}
	}
	point current_position(initial_x, 0);
	auto in_bounds = [&map](const point& p) {
		if((p.x < 0)||(p.y < 0)) {
			return false;
		}
		if((p.x >= (int)map.width())||(p.y >= (int)map.height())) {
			return false;
		}
		return true;
	};

	int current_direction = 0;
	bool flip = false;
	while(true) {
		if(map(current_position.x, current_position.y) == '+') {
			// We need to turn
			int new_dir = (current_direction+1)%2;
			bool first = true;
			std::vector<point> possibilities;
			possibilities.push_back(current_position+dirs[new_dir]);
			possibilities.push_back(current_position-dirs[new_dir]);
			point next;
			for(point& pos: possibilities) {
				if(in_bounds(pos)&&(map(pos.x,pos.y) != ' ')) {
					next = pos;

					break;
				}
				if(first) {
					first = false;
				}
			}
			if (next == point()) {
				// Nowhere to go!
				break;
			} else {
				// Advance position
				current_position = next;
				current_direction = new_dir;
				if(first) {
					flip = false;
				} else {
					flip = true;
				}
			}
		} else {
			// continue going straight
			point next;
			if(flip) {
				next = current_position-dirs[current_direction];
			} else {
				next = current_position+dirs[current_direction];
			}
			if(in_bounds(next) && (map(next.x, next.y) != ' ')) {
				// Advance position
				current_position = next;
			} else {
				// Nowhere to go!
				break;
			}
		}
		steps += 1;
		if((map(current_position.x, current_position.y) >= 'A')&&(map(current_position.x, current_position.y) <= 'Z')) {
			if(verbose) {
				std::cout << "Encountered letter " << map(current_position.x, current_position.y) << std::endl;
			}
			answer.push_back(map(current_position.x, current_position.y));
		}
	};

	return std::pair<std::string,int>(answer,steps);
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 19");
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

    std::vector<std::string> map_lines;
    std::string line;
    while(std::getline(infile, line)) {
        map_lines.push_back(line);
    }

	size_t width = map_lines[0].size();
	size_t length = map_lines.size();
	array_2d<char> map(width, length);

	// Initialize map
	for(size_t iy = 0; iy < length; ++iy) {
		for(size_t ix = 0; ix < width; ++ix) {
			map.assign(ix,iy) = map_lines[iy][ix];
		}
	}

	if(verbose) {
		std::cout << "Got map with dimensions: " << map.width() << "," << map.height() << std::endl;
	}

	auto map_answer = navigate_map(map, verbose);
	std::cout << "Task 1: " << map_answer.first << std::endl;
	std::cout << "Task 2: " << map_answer.second+1 << std::endl;

	return 0;
}
