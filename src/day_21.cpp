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
                    for(int row_j = 0; row_j < N; ++row_j) {
                        if(first) {
                            ss << '/';
                            first = false;
                        }
                        ss << '.';
                    }
                }
                input = ss.str();
            }
            if(input.size() != (N*N+(N-1))) {
                std::cerr << "Incorrect size of input string!" << std::endl;
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
        specific_grid(const specific_grid<N>& rhs) {
            copy(rhs);
        }
        specific_grid<N>& operator=(const specific_grid<N>& rhs) {
            copy(rhs);
            return (*this);
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

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 21");
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
    std::regex rule_pattern("^([.#/]+) => ([.#/]+)$");
    std::map<specific_grid<2>,specific_grid<3>> map_2_dict;
    std::map<specific_grid<3>,specific_grid<4>> map_3_dict;
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

	return 0;
}
