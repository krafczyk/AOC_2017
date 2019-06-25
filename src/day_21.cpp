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
        template<int M>
        specific_grid(const specific_grid<M>& larger, int b_row, int b_col) {
            for(int row_i = 0; row_i < N; ++row_i) {
                for(int col_i = 0; col_i < N; ++col_i) {
                    grid[row_i][col_i] = larger(b_row+row_i,b_col+col_i);
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
    // grid definitions
    std::unordered_map<int,std::vector<specific_grid<2>>> s2_grids;
    std::unordered_map<int,std::vector<specific_grid<3>>> s3_grids;
    // Map which s3 tile unique s2 tiles map to 
    std::unordered_map<int,specific_grid<3>> s2_map;
    // Map which s4 tile unique s3 tiles map to
    std::unordered_map<int,specific_grid<4>> s3_map;
    // Store a dictionary of what tiles break into other tiles.
    std::unordered_map<int,std::unordered_map<int,int>> tile_mapping;
    // Store a dictionary of how many points each tile has
    std::unordered_map<int,int> point_map;
    // The current state of the grid
    std::vector<int> grid_tiles;
    // The number of tiles there are 
    std::vector<int> points_on;
    int num_2 = 0;
    int num_3 = 0;
    int num_tiles = 0;
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
            // We have a definition for a size 2 tile
            num_2 += 1;
            specific_grid<2> left_grid(left_rule);
            specific_grid<3> answer_grid(right_rule);
            std::vector<specific_grid<2>> Left = all_unique(left_grid);
            // Add thew new tile definition and advance tile counter.
            s2_grids[num_tiles] = Left;
            s2_map[num_tiles] = answer_grid;
            num_tiles += 1;
        } else if (left_rule.size() == 11) {
            num_3 += 1;
            specific_grid<3> left_grid(left_rule);
            specific_grid<4> answer_grid(right_rule);
            std::vector<specific_grid<3>> Left = all_unique(left_grid);
            s3_grids[num_tiles] = Left;
            s3_map[num_tiles] = answer_grid;
            num_tiles += 1;
        } else {
            std::cerr << "Got a starting rule which isn't correct!" << std::endl;
        }
    }

    // Complete tile mapping for 2d tiles.
    std::cout << "s2 tile map building" << std::endl;
    for(auto& v: s2_map) {
        int s3i = 0;
        std::cout << "s3 tile we are trying to match:" << std::endl;
        std::cout << v.second;
        for(auto& v2: s3_grids) {
            std::cout << "Trying to match against grid " << v2.first << std::endl;
            for(auto& v3: v2.second) {
                std::cout << v3 << std::endl;
            }
            if(hasElement(v2.second,v.second)) {
                std::cout << "This break reached" << std::endl;
                s3i = v2.first;
                break;
            }
        }
        tile_mapping[v.first][s3i] = 1;
    }
    // Complete tile mapping for 3d tiles
    for(auto& v: s3_map) {
        // Determine components of the s4 tile
        std::unordered_map<int,int> component_submap;
        specific_grid<4>& matched_tile = v.second;
        std::vector<specific_grid<2>> to_test;
        to_test.push_back(specific_grid<2>(matched_tile, 0,0));
        to_test.push_back(specific_grid<2>(matched_tile, 0,1));
        to_test.push_back(specific_grid<2>(matched_tile, 1,0));
        to_test.push_back(specific_grid<2>(matched_tile, 1,1));
        for(auto& v2: to_test) {
            for(auto& v3: s2_grids) {
                if(hasElement(v3.second, v2)) {
                    component_submap[v3.first] += 1;
                }
            }
        }
        for(auto& v2: component_submap) {
            tile_mapping[v.first][v2.first] = v2.second;
        }
    }

    // Initialize the point map
    for(auto& v: s2_grids) {
        point_map[v.first] = v.second[0].num();
    }
    for(auto& v: s3_grids) {
        point_map[v.first] = v.second[0].num();
    }

    if(verbose) {
        std::cout << "Basics Summary" << std::endl;
        std::cout << "Unique tiles of size 2" << std::endl;
        for(auto& v: s2_grids) {
            std::cout << "Grid " << v.first << ":" << std::endl;
            for(auto& g: v.second) {
                std::cout << g << std::endl;
            }
        }
        std::cout << "Unique tiles of size 3" << std::endl;
        for(auto& v: s3_grids) {
            std::cout << "Grid " << v.first << ":" << std::endl;
            for(auto& g: v.second) {
                std::cout << g << std::endl;
            }
        }
        std::cout << "The tile mapping: " << std::endl;
        for(auto& v: tile_mapping) {
            std::cout << v.first << " -> ";
            for(auto &v2: v.second) {
                std::cout << v2.first << ":" << v2.second << " ";
            }
            std::cout << std::endl;
        }
    }

    // Initial map position
    specific_grid<3> init_grid(".#./..#/###");
    std::unordered_map<int,int> full_grid;
    std::unordered_map<int,int> temp_grid;
    for(auto& v: s3_grids) {
        if(hasElement(v.second,init_grid)) {
            full_grid[v.first] = 1;
        }
    }

    auto num_on = [&point_map](std::unordered_map<int,int>& grid) {
        int total = 0;
        for(auto v: grid) {
            total += point_map[v.first]*v.second;
        }
        return total;
    };

    int it = 0;
    if(verbose) {
        std::cout << "Iteration " << it << " " << num_on(full_grid) << " points on" << std::endl;
    }
    while(it < num_iterations) {
        std::swap(full_grid, temp_grid);
        full_grid.clear();
        for(auto& v: temp_grid) {
            for(auto& v2: tile_mapping[v.first]) {
                full_grid[v2.first] = v.second*v2.second;
            }
        }
        ++it;
        if(verbose) {
            std::cout << "Iteration " << it << " " << num_on(full_grid) << " points on" << std::endl;
        }
    }

    std::cout << "Task 1: There were " << num_on(full_grid) << " pixels which were on." << std::endl;

	return 0;
}
