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

template<int N>
class specific_grid {
    public:
        specific_grid(std::string input="") {
            if(input == "") {
                // Build default grid string
                std::stringstream ss;
                bool first = true;
                for(int row_i = 0; row_i < N; ++row_i) {
                    if(!first) {
                        ss << '/';
                    }
                    if(first) {
                        first = false;
                    }
                    for(int col_i = 0; col_i < N; ++col_i) {


                        ss << '.';
                    }
                }
                input = ss.str();
            }
            if(input.size() != (N*N+(N-1))) {
                std::cerr << "Incorrect size of input string! got " << input.size() << " expected " << (N*N+(N-1)) << std::endl;
                std::cerr << "String was (" << input << ")" << std::endl;
                throw;
            }
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    char c = input[(N+1)*row_i+col_i];
                    if (c == '.') {
                        grid[row_i][col_i] = false;
                    } else if (c == '#') {
                        grid[row_i][col_i] = true;
                    } else {
                        std::cerr << "Got an unexpected character!" << std::endl;
                        throw;
                    }
                }
            }
        }
        specific_grid(const array_2d<bool>& full, int b_row, int b_col) {
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    grid[row_i][col_i] = full(b_row+row_i,b_col+col_i);
                }
            }
        }
        specific_grid(const specific_grid<N>& rhs) {
            copy(rhs);
        }
        specific_grid<N>& operator=(const specific_grid<N>& rhs) {
            copy(rhs);
            return (*this);
        }
        bool operator()(int row_i, int col_i) const {
            return grid[row_i][col_i];
        }
        int num() const {
            int total = 0;
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    if(grid[row_i][col_i]) {
                        total += 1;
                    }
                }
            }
            return total;
        }
        bool operator==(const specific_grid<N>& rhs) const {
            bool matched = true;
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    if(grid[row_i][col_i] != rhs.grid[row_i][col_i]) {
                        matched = false;
                        break;
                    }
                }
            }
            return matched;
        }
        specific_grid<N>& rotate() {
            // A clockwise rotation
            bool temp[N][N];
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    temp[row_i][col_i] = grid[row_i][col_i];
                }
            }
            // rotation
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    grid[col_i][N-row_i-1] = temp[row_i][col_i];
                }
            }
            return *this;
        }
        specific_grid<N>& flip() {
            // A vertical flip
            for(int col_i = 0; col_i < N; ++col_i) {
                for(int row_i = 0; row_i < N/2; ++row_i) {
                    bool temp = grid[row_i][col_i];
                    grid[row_i][col_i] = grid[N-row_i-1][col_i];
                    grid[N-row_i-1][col_i] = temp;
                }
            }
            return *this;
        }
        std::string hash() const {
            std::stringstream ss;
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    if(grid[row_i][col_i]) {
                        ss << '#';
                    } else {
                        ss << '.';
                    }
                }
            }
            return ss.str();
        }
        friend std::ostream& operator<<(std::ostream& out, const specific_grid<N>& grid) {
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    if(grid.grid[row_i][col_i]) {
                        out << '#';
                    } else {
                        out << '.';
                    }
                }
                out << std::endl;
            }
            return out;
        }
    private:
        void copy(const specific_grid<N>& rhs) {
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    grid[row_i][col_i] = rhs.grid[row_i][col_i];
                }
            }
        }
        bool grid[N][N];
};

template<int N>
class specific_grid_hasher {
    public:
        size_t operator()(const specific_grid<N>& k) const {
            return std::hash<std::string>()(k.hash());
        }
};

template<int N>
std::vector<specific_grid<N>> all_unique(const specific_grid<N>& in) {
    specific_grid<N> temp(in);
    std::vector<specific_grid<N>> answer;
    answer.push_back(in);
    auto test_and_add = [&answer](const specific_grid<N>& in) {
        if(!hasElement(answer, in)) {
            answer.push_back(in);
        }
    };
    test_and_add(temp.rotate());
    test_and_add(temp.rotate());
    test_and_add(temp.rotate());
    test_and_add(temp.rotate());
    test_and_add(temp.flip());
    test_and_add(temp.rotate());
    test_and_add(temp.rotate());
    test_and_add(temp.rotate());
    test_and_add(temp.rotate());
    return answer;
}

void show(std::ostream& out, const array_2d<bool>& map) {
    for(int row_i = 0; row_i < (int)map.height(); ++row_i) {
        for(int col_i = 0; col_i < (int)map.width(); ++col_i) {
            if (map(row_i,col_i)) {
                out << '#';
            } else {
                out << '.';
            }
        }
        out << std::endl;
    }
    out << std::endl;
}

int sum(const array_2d<bool>& map) {
    int total = 0;
    for(int row_i = 0; row_i < (int) map.width(); ++row_i) {
        for(int col_i = 0; col_i < (int) map.height(); ++col_i) {
            if(map(row_i,col_i)) {
                total += 1;
            }
        }
    }
    return total;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    int num_iterations = 0;
	ArgParse::ArgParser Parser("Day 21");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-n/--num-iterations", "The number of iterations to try", &num_iterations);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
    std::regex rule_pattern("^([.#/]+) => ([.#/]+)$");
    std::unordered_map<specific_grid<2>,specific_grid<3>,specific_grid_hasher<2>> map_2_dict;
    std::unordered_map<specific_grid<3>,specific_grid<4>,specific_grid_hasher<3>> map_3_dict;
    std::string line;
	std::ifstream infile(input_filepath);
    while(std::getline(infile, line)) {
        std::smatch rule_match;
        if(!std::regex_match(line, rule_match, rule_pattern)) {
            std::cerr << "Encounter an incorrectly formatted rule!" << std::endl;
            return 1;
        }
        std::string left_rule = rule_match[1].str();
        std::string right_rule = rule_match[2].str();
        std::cout << "Left rule: (" << left_rule << ") ";
        std::cout << "Right rule: (" << right_rule << ")" << std::endl;
        if(left_rule.size() == 5) {
            specific_grid<2> left_grid(left_rule);
            specific_grid<3> answer_grid(right_rule);
            std::vector<specific_grid<2>> Left = all_unique(left_grid);
            for(specific_grid<2>& g: Left) {
                map_2_dict[g] = answer_grid;
            }
        } else if (left_rule.size() == 11) {
            specific_grid<3> left_grid(left_rule);
            specific_grid<4> answer_grid(right_rule);
            std::vector<specific_grid<3>> Left = all_unique(left_grid);
            for(specific_grid<3>& g: Left) {
                map_3_dict[g] = answer_grid;
            }
        } else {
            std::cerr << "Got a starting rule which isn't correct!" << std::endl;
        }
    }

    // Initialize full grid position
    array_2d<bool>* full_grid = new array_2d<bool>(3,3);
    full_grid->assign(0,0) = false;
    full_grid->assign(0,1) = true;
    full_grid->assign(0,2) = false;
    full_grid->assign(1,0) = false;
    full_grid->assign(1,1) = false;
    full_grid->assign(1,2) = true;
    full_grid->assign(2,0) = true;
    full_grid->assign(2,1) = true;
    full_grid->assign(2,2) = true;

    int it = 0;
    if(verbose) {
        std::cout << "Iteration " << it << std::endl;
        show(std::cout, *full_grid);
    }

    while(it < num_iterations) {
        if(full_grid->height()%3 == 0) {
            // We have a divisible by 3 component.
            // Initialize a new map
            int new_N = ((int)full_grid->height()/3)*4;
            array_2d<bool>* new_grid = new array_2d<bool>(new_N,new_N);
            for(int rti = 0; rti < ((int)full_grid->height())/3; ++rti) {
                for(int cti = 0; cti < ((int)full_grid->width())/3; ++cti) {
                    // Extract tile
                    specific_grid<3> tile(*full_grid,rti*3,cti*3);
                    // Find replacement
                    specific_grid<4>& new_tile = map_3_dict[tile];
                    // Set new grid values
                    for(int row_i = 0; row_i < 4; ++row_i) {
                        for(int col_i = 0; col_i < 4; ++col_i) {
                            new_grid->assign(rti*4+row_i,cti*4+col_i) = new_tile(row_i,col_i);
                        }
                    }
                }
            }
            // Swap arrays
            delete full_grid;
            full_grid = new_grid;
        } else {
            // Use the other method.
            // Initialize a new map
            int new_N = ((int)full_grid->height()/2)*3;
            array_2d<bool>* new_grid = new array_2d<bool>(new_N,new_N);
            for(int rti = 0; rti < ((int)full_grid->height())/2; ++rti) {
                for(int cti = 0; cti < ((int)full_grid->width())/2; ++cti) {
                    // Extract tile
                    specific_grid<2> tile(*full_grid,rti*2,cti*2);
                    // Find replacement
                    specific_grid<3>& new_tile = map_2_dict[tile];
                    // Set new grid values
                    for(int row_i = 0; row_i < 3; ++row_i) {
                        for(int col_i = 0; col_i < 3; ++col_i) {
                            new_grid->assign(rti*3+row_i,cti*3+col_i) = new_tile(row_i,col_i);
                        }
                    }
                }
            }
            // Swap arrays
            delete full_grid;
            full_grid = new_grid;
        }
        ++it;
        if(verbose) {
            std::cout << "Iteration " << it << std::endl;
            show(std::cout, *full_grid);
        }
    }

    std::cout << "Task 1: There were " << sum(*full_grid) << " pixels which were on." << std::endl;

    delete full_grid;

	return 0;
}
