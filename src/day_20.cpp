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

class point: public point3d<type_t> {
    public:
        point(type_t x=0, type_t y=0, type_t z=0) : point3d<type_t>(x,y,z) {}
        type_t dist(const point& p) const {
            return std::abs(this->x-p.x)+std::abs(this->y-p.y)+std::abs(this->x-p.z);
        }
        type_t mag() const {
            return std::abs(x)+std::abs(y)+std::abs(z);
        }
};

class particle {
    public:
        particle(int id) { this->id = id;}
        void set_p(const point& in) {
            this->p = in;
        }
        void set_v(const point& in) {
            this->v = in;
        }
        void set_a(const point& in) {
            this->a = in;
        }
        point p;
        point v;
        point a;
        int id;
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
    std::regex particle_match("^p=<([\\-0-9]+),([\\-0-9]+),([\\-0-9]+)>, v=<([\\-0-9]+),([\\-0-9]+),([\\-0-9]+)>, a=<([\\-0-9]+),([\\-0-9]+),([\\-0-9]+)>$");
    std::vector<particle> particles;
	std::ifstream infile(input_filepath);
    std::string line;
    int particle_id = 0;
    auto build_point = [](std::smatch& match, int offset) {
        return point(fetch_value<type_t>(match[1+offset]),
                     fetch_value<type_t>(match[2+offset]),
                     fetch_value<type_t>(match[3+offset]));
    };
    while(std::getline(infile, line)) {
        std::smatch match;
        if(!std::regex_match(line, match, particle_match)) {
            std::cerr << "This line doesn't conform to the expected format!" << std::endl;
            return -1;
        }
        particle p(particle_id);
        particle_id += 1;
        p.set_p(build_point(match, 0));
        p.set_v(build_point(match, 3));
        p.set_a(build_point(match, 6));
        particles.push_back(p);
    }

    if(verbose) {
        std::cout << "We read in " << particles.size() << " particles" << std::endl;
    }

    std::sort(particles.begin(), particles.end(), [](const particle& a, const particle& b) {
        return (a.a.mag() < b.a.mag());
    });

    if(verbose) {
        std::cout << "Particle accelerations after sorting" << std::endl;
        for(const particle& p: particles) {
            std::cout << p.id << ": " << p.a.mag() << std::endl;
        }
    }

    std::cout << "Task 1: closest particle will be: " << particles[0].id << std::endl;

	return 0;
}
