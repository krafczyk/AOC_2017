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
        bool operator==(const point& p) const {
            return point3d<type_t>::operator==(p);
        }
        bool operator!=(const point& p) const {
            return point3d<type_t>::operator!=(p);
        }
        type_t dist(const point& p) const {
            return std::abs(this->x-p.x)+std::abs(this->y-p.y)+std::abs(this->x-p.z);
        }
        type_t mag() const {
            return std::abs(x)+std::abs(y)+std::abs(z);
        }
};

type_t compute(type_t x0, type_t v0, type_t a0, type_t i) {
    return x0+(i*v0)+((i*(i+1)/2)*a0);
}

type_t imax_compute(type_t x0, type_t v0, type_t a0, type_t f) {
    if(a0 != 0) {
        return ((2*std::abs(v0)*f)/std::abs(a0))-1;
    } else {
        return (f*std::abs(x0))/std::abs(v0);
    }
}

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
        void compute_pt(type_t i) {
            pt.x = compute(p.x,v.x,a.x,i);
            pt.y = compute(p.y,v.y,a.y,i);
            pt.y = compute(p.y,v.y,a.y,i);
        }
        type_t imax(type_t f) const {
            type_t imax = std::numeric_limits<type_t>::min();
            imax = std::max(imax, imax_compute(p.x,v.x,a.x,f));
            imax = std::max(imax, imax_compute(p.y,v.y,a.y,f));
            imax = std::max(imax, imax_compute(p.z,v.z,a.z,f));
            return imax;
        }
        const point& get_pt() const {
            return this->pt;
        }
        point p;
        point v;
        point a;
        point pt;
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

    //if(verbose) {
    //    std::cout << "Particle accelerations after sorting" << std::endl;
    //    for(const particle& p: particles) {
    //        std::cout << p.id << ": " << p.a.mag() << std::endl;
    //    }
    //}

    std::cout << "Task 1: closest particle will be: " << particles[0].id << std::endl;

    // We now need to determine the max step i to simulate
    type_t i_max = std::numeric_limits<type_t>::min();
    type_t factor = 20;
    for(const particle& p: particles) {
        i_max = std::max(i_max, p.imax(factor));
    }

    std::cout << "We will need to simulate at least " << i_max << " Steps to achieve the separation from factor " << factor << std::endl;

    // Simulate the particles.
    for(type_t i = 1; i <= i_max; ++i) {
        // Compute new positions for all particles.
        for(particle& p: particles) {
            p.compute_pt(i);
        }
        std::set<int> collided;
        // Detect collided particles.
        for(size_t idx_i = 0; idx_i < particles.size()-1; ++idx_i) {
            for(size_t idx_j = idx_i+1; idx_j < particles.size(); ++idx_j) {
                particle& pi = particles[idx_i];
                particle& pj = particles[idx_j];
                if(pi.pt == pj.pt) {
                    // Detected a collision!
                    collided.insert(pi.id);
                    collided.insert(pj.id);
                }
            }
        }
        // Remove the collided particles.
        for(int id: collided) {
            particles.erase(std::find_if(particles.begin(), particles.end(), [&id](const particle& p) { return (p.id == id); }));
        }
    }

    std::cout << "Task 2: There were " << particles.size() << " left over" << std::endl;

	return 0;
}
