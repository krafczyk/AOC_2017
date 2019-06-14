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

typedef unsigned char knot_t;

void hash_round(std::vector<knot_t>& list, const std::vector<knot_t>& lengths, size_t& current_pos, size_t& skip, size_t size) {
    // Perform 
    size_t it = 0;
    while(it < lengths.size()) {
        size_t len = lengths[it];
        // Swap elements until we get into the middle.
        for(size_t I = 0; I < len/2; ++I) {
            size_t i = (current_pos+I)%size;
            size_t j = (current_pos+len-I-1)%size;
            knot_t temp = list[i];
            list[i] = list[j];
            list[j] = temp;
        }

        // Move current_position
        current_pos = (current_pos+len+skip)%size;
        skip += 1;
        it += 1;
    }

}

std::string knot_hash(const std::string& input_string, size_t size = 256) {
    // Initialize the list
    std::vector<knot_t> list;
    for(size_t i = 0; i < size; ++i) {
        list.push_back((knot_t) i);
    }
    // Build up the input lengths
    std::vector<knot_t> input_lengths;
    for(size_t i = 0; i < input_string.size(); ++i) {
        input_lengths.push_back((knot_t)input_string[i]);
    }
    input_lengths.push_back(17);
    input_lengths.push_back(31);
    input_lengths.push_back(73);
    input_lengths.push_back(47);
    input_lengths.push_back(23);

    size_t skip = 0;
    size_t current_pos = 0;
    size_t n_it = 0;
    while(n_it < 64) {
        hash_round(list, input_lengths, current_pos, skip, size);
        n_it += 1;
    }
    std::vector<knot_t> dense_hash;
    for(size_t i = 0; i < 16; ++i) {
        knot_t val = list[i*16];
        for(size_t j = 1; j < 16; ++j) {
            val = val^list[i*16+j];
        }
        dense_hash.push_back(val);
    } std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for(knot_t& v: dense_hash) {
        ss << std::setw(2) << static_cast<unsigned>(v);
    }
    return ss.str();
}

std::string hex_char_to_bin(char c) {
    switch(c) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'a': return "1010";
        case 'b': return "1011";
        case 'c': return "1100";
        case 'd': return "1101";
        case 'e': return "1110";
        case 'f': return "1111";
    }
    return "";
}

std::string binary_hash(const std::string& str) {
    std::string answer;
    for(size_t i = 0; i < str.size(); ++i) {
        answer = answer+hex_char_to_bin(str[i]);
    }
    return answer;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_key;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task Template");
	Parser.AddArgument("-ik/--input-key", "File defining the input", &input_key);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    std::string hash = knot_hash(input_key);

    if(verbose) {
        std::cout << "disk status: " << std::endl;
        for(size_t i = 0; i < 128; ++i) {
            std::stringstream ss;
            ss << input_key << "-" << i;
            std::string row_hash = knot_hash(ss.str());
            std::string binary_row = binary_hash(row_hash);
            std::replace(binary_row.begin(), binary_row.end(), '0', '.');
            std::replace(binary_row.begin(), binary_row.end(), '1', '#');
            std::cout << binary_row << std::endl;
        }
    }

    typedef std::pair<int,int> point;

    size_t used_squares = 0;
    bool map[128][128];
    std::vector<point> positions;
    for(size_t i = 0; i < 128; ++i) {
        std::stringstream ss;
        ss << input_key << "-" << i;
        std::string row_hash = knot_hash(ss.str());
        std::string binary_row = binary_hash(row_hash);
        for(size_t j = 0; j < 128; ++j) {
            if(binary_row[j] == '1') {
                used_squares += 1;
                map[i][j] = true;
                positions.push_back(std::pair<int,int>(i,j));
            } else {
                map[i][j] = false;
            }
        }
    }

    point dirs[4] = {
        {1,0},
        {0,1},
        {-1,0},
        {0,-1}
    };

    size_t num_regions = 0;
    while(positions.size() != 0) {
        std::vector<point> edge_points;
        edge_points.push_back(positions.back());
        positions.pop_back();
        while(edge_points.size() != 0) {
            point p = edge_points.back();
            edge_points.pop_back();
            for(size_t d = 0; d < 4; ++d) {
                point np = point(p.first+dirs[d].first,p.second+dirs[d].second);
                // Skip points beyond boundaries
                if((np.first < 0)||(np.first >= 128)||
                   (np.second < 0)||(np.second >= 128)) {
                    continue;
                }
                // skip non used points
                if(!map[np.first][np.second]) {
                    continue;
                }
                // Skip points not in positions and not already in the edge points
                if((!hasElement(positions, np))||hasElement(edge_points, np)) {
                    continue;
                }
                edge_points.push_back(np);
                positions.erase(std::find(positions.begin(), positions.end(), np));
            }
        }
        num_regions += 1;
    }

    std::cout << "Task 1: There were " << used_squares << " used squares." << std::endl;
    std::cout << "Task 2: There were " << num_regions << " regions." << std::endl;

	return 0;
}
