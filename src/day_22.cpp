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
        type_t hash() const {
            return pair_hash_l(this->x, this->y);
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

const int Clean = 0;
const int Weakened = 1;
const int Infected = 2;
const int Flagged = 3;

char stat_c[4] = { '.', 'W', '#', 'F' };
std::string dir_str[4] = { "right", "up", "left", "down" };

int inc_state(int S) {
    return (S+1)%4;
}

void print_state(const point& current_point, const int current_dir, const std::map<type_t,int>& state_map, const std::map<type_t,point>& point_map) {
    // Determine extents
    type_t min_x = std::numeric_limits<type_t>::max();
    type_t max_x = std::numeric_limits<type_t>::min();
    type_t min_y = std::numeric_limits<type_t>::max();
    type_t max_y = std::numeric_limits<type_t>::min();
    for(auto& P: point_map) {
        const point& p =  P.second;
        max_x = std::max(p.x,max_x);
        min_x = std::min(p.x,min_x);
        max_y = std::max(p.y,max_y);
        min_y = std::min(p.y,min_y);
    }
    max_x = std::max(current_point.x,max_x);
    max_y = std::max(current_point.y,max_y);
    min_x = std::min(current_point.x,min_x);
    min_y = std::min(current_point.y,min_y);
    // Report current direction
    std::cout << "at " << current_point << " going " << dir_str[current_dir] << std::endl;
    for(type_t y = max_y; y >= min_y; --y) {
        for(type_t x = min_x; x <= max_x; ++x) {
            bool left_space = true;
            if(x > min_x) {
                if(current_point == point(x-1,y)) {
                    std::cout << "]";
                    left_space = false;
                }
            }
            if (current_point == point(x,y)) {
                std::cout << "[";
                left_space = false;
            }
            if(left_space) {
                std::cout << " ";
            }
            try {
                std::cout << stat_c[state_map.at(point(x,y).hash())];
            } catch (std::out_of_range& e) {
                std::cout << stat_c[Clean];
            }
            if(x == max_x) {
                if(current_point == point(x,y)) {
                    std::cout << "]";
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose1 = false;
	bool verbose2 = false;
    int num_1 = 10000;
    int num_2 = 10000000;
	ArgParse::ArgParser Parser("Day 22");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v1/--verbose1", "Print Verbose output", &verbose1);
	Parser.AddArgument("-v2/--verbose2", "Print Verbose output", &verbose2);
    Parser.AddArgument("-n1/--num-1", "Number of iterations 1", &num_1);
    Parser.AddArgument("-n2/--num-2", "Number of iterations 2", &num_2);

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
    std::set<point> initially_infected;
    type_t height = input_data.size();
    type_t width = input_data[0].size();
    for(int row_i = 0; row_i < height; ++row_i) {
        for(int col_i = 0; col_i < width; ++col_i) {
            if(input_data[row_i][col_i] == '#') {
                initially_infected.insert(point(col_i-(width/2),(height/2)-row_i));
            }
        }
    }

    if(verbose1||verbose2) {
        std::cout << "Initial infected points" << std::endl;
        for(auto& p: initially_infected) {
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
    auto reverse = [&current_dir]() {
        current_dir = (current_dir+2)%4;
    };

    std::set<point> infected = initially_infected;
    int num = 0;
    int num_infected = 0;
    while(num < num_1) {
        auto pos_it = std::find(infected.begin(), infected.end(), current_position);
        if(pos_it == infected.end()) {
            // Not infected.
            num_infected += 1;
            infected.insert(current_position);
            turn_left();
        } else {
            // infected
            infected.erase(pos_it);
            turn_right();
        }
        // advance position
        current_position = current_position+dirs[current_dir];
        num += 1;
    }

    if(verbose1) {
        std::cout << "Final state" << std::endl;
        for(auto& p: infected) {
            std::cout << p << std::endl;
        }
    }

    std::cout << "Task 1: " << num_infected << " computers got infected" << std::endl;

    std::map<type_t,int> state_map;
    std::map<type_t,point> hash_map;

    for(const point& p: initially_infected) {
        state_map[p.hash()] = Infected;
        hash_map[p.hash()] = p;
    }

    current_position = point(0,0);
    current_dir = 1;
    num = 0;
    num_infected = 0;
    while(num < num_2) {
        int& the_state = state_map[current_position.hash()];
        hash_map[current_position.hash()] = current_position;
        if((the_state < 0)||(the_state > 3)) {
            std::cerr << "Encountered a strange state!: " << the_state << std::endl;
            std::cerr << "The position was: " << current_position << std::endl;
            throw;
        }
        std::cout << "Current " << current_position << " " << dirs[current_dir] << " s: " << the_state << "(" << stat_c[the_state] << ")";
        if(the_state == Clean) {
            turn_left();
        } else if (the_state == Weakened) {
        } else if (the_state == Infected) {
            num_infected += 1;
            turn_right();
        } else if (the_state == Flagged) {
            reverse();
        } else {
            std::cerr << "Encountered an unknown state " << the_state << std::endl;
            throw;
        }
        // Increment state
        the_state = inc_state(the_state);
        std::cout << " -> " << the_state << "(" << stat_c[the_state] << ")" << std::endl;
        // Advance position
        current_position = current_position+dirs[current_dir];
        num += 1;
    }

    if(verbose2) {
        std::cout << state_map.size() << " nodes were either visited or have non-clean state" << std::endl;
        for(const auto& v: state_map) {
            if(v.second > 4) {
                std::cout << "Incorrect state!" << std::endl;
            } else {
                std::cout << hash_map[v.first] << " " << stat_c[v.second] << std::endl;
            }
        }
        print_state(current_position, current_dir, state_map, hash_map);
    }

    std::cout << "Task 2: " << num_infected << " computers got infected" << std::endl;

	return 0;
}
