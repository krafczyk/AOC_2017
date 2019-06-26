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
typedef std::unordered_map<std::string,type_t> registers;
const std::string ip = "ip";

size_t num_mul = 0;

class instruction {
    public:
        virtual ~instruction() {}
        virtual bool operator()(registers& r) = 0;
};

class inst_2v: public instruction {
    public:
        inst_2v(const std::string& reg, type_t v) {
            this->reg = reg;
            this->v = v;
        }
        virtual ~inst_2v() {}
        virtual bool operator()(registers& r) = 0;
    protected:
        std::string reg;
        type_t v;
};

class inst_2r: public instruction {
    public:
        inst_2r(const std::string& regl, const std::string& regr) {
            this->regl = regl;
            this->regr = regr;
        }
        virtual ~inst_2r() {}
        virtual bool operator()(registers& r) = 0;
    protected:
        std::string regl;
        std::string regr;
};

class inst_1v: public instruction {
    public:
        inst_1v(type_t v) {
            this->v = v;
        }
        virtual ~inst_1v() {}
        virtual bool operator()(registers& r) = 0;
    protected:
        type_t v;
};

class inst_1r: public instruction {
    public:
        inst_1r(const std::string& reg) {
            this->reg = reg;
        }
        virtual ~inst_1r() {}
        virtual bool operator()(registers& r) = 0;
    protected:
        std::string reg;
};

class setv: public inst_2v {
    public:
        setv(const std::string& reg, type_t v) : inst_2v(reg, v) {}
        bool operator()(registers& r) {
            r[reg] = v;
            return true;
        }
};

class setr: public inst_2r {
    public:
        setr(const std::string& regl, const std::string& regr) : inst_2r(regl, regr) {}
        bool operator()(registers& r) {
            r[regl] = r[regr];
            return true;
        }
};

class addv: public inst_2v {
    public:
        addv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r) {
            r[reg] += v;
            return true;
        }
};

class addr: public inst_2r {
    public:
        addr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r) {
            r[regl] += r[regr];
            return true;
        }
};

class subv: public inst_2v {
    public:
        subv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r) {
            r[reg] -= v;
            return true;
        }
};

class subr: public inst_2r {
    public:
        subr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r) {
            r[regl] -= r[regr];
            return true;
        }
};

class mulv: public inst_2v {
    public:
        mulv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r) {
            num_mul += 1;
            r[reg] *= v;
            return true;
        }
};

class mulr: public inst_2r {
    public:
        mulr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r) {
            num_mul += 1;
            r[regl] *= r[regr];
            return true;
        }
};

class modv: public inst_2v {
    public:
        modv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r) {
            r[reg] %= v;
            return true;
        }
};

class modr: public inst_2r {
    public:
        modr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r) {
            r[regl] %= r[regr];
            return true;
        }
};

class jgzv: public inst_2v {
    public:
        jgzv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r) {
            if(r[reg] > 0) {
                r[ip] += (v-1);
            }
            return true;
        }
};

class jgzr: public inst_2r {
    public:
        jgzr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r) {
            if(r[regl] > 0) {
                r[ip] += (r[regr]-1);
            }
            return true;
        }
};

class jnzv: public inst_2v {
    public:
        jnzv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r) {
            if(r[reg] != 0) {
                r[ip] += (v-1);
            }
            return true;
        }
};

class jnzr: public inst_2r {
    public:
        jnzr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r) {
            if(r[regl] != 0) {
                r[ip] += (r[regr]-1);
            }
            return true;
        }
};

bool is_register(const std::string& str) {
    for(size_t idx = 0; idx < str.size(); ++idx) {
        char c = str[idx];
        if(((c >= 'a')&&(c <= 'z'))||((c >= 'A')&&(c <= 'Z'))) {
            return true;
        }
    }
    return false;
}

instruction* InstructionFactory(const std::vector<std::string>& words) {
    if (words[0] == "set") {
        if(is_register(words[2])) {
            return (instruction*) new setr(words[1], words[2]);
        } else {
            return (instruction*) new setv(words[1], fetch_value<type_t>(words[2]));
        }
    } else if (words[0] == "add") {
        if(is_register(words[2])) {
            return (instruction*) new addr(words[1], words[2]);
        } else {
            return (instruction*) new addv(words[1], fetch_value<type_t>(words[2]));
        }
    } else if (words[0] == "sub") {
        if(is_register(words[2])) {
            return (instruction*) new subr(words[1], words[2]);
        } else {
            return (instruction*) new subv(words[1], fetch_value<type_t>(words[2]));
        }
    } else if (words[0] == "mul") {
        if(is_register(words[2])) {
            return (instruction*) new mulr(words[1], words[2]);
        } else {
            return (instruction*) new mulv(words[1], fetch_value<type_t>(words[2]));
        }
    } else if (words[0] == "mod") {
        if(is_register(words[2])) {
            return (instruction*) new modr(words[1], words[2]);
        } else {
            return (instruction*) new modv(words[1], fetch_value<type_t>(words[2]));
        }
    } else if (words[0] == "jgz") {
        if(is_register(words[2])) {
            return (instruction*) new jgzr(words[1], words[2]);
        } else {
            return (instruction*) new jgzv(words[1], fetch_value<type_t>(words[2]));
        }
    } else if (words[0] == "jnz") {
        if(is_register(words[2])) {
            return (instruction*) new jnzr(words[1], words[2]);
        } else {
            return (instruction*) new jnzv(words[1], fetch_value<type_t>(words[2]));
        }
    } else {
        std::cerr << "Encountered an unknown command!" << std::endl;
        throw;
    }
    std::cerr << "We shouldn't get to here!" << std::endl;
    throw;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Day 23");
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

    std::vector<instruction*> program;
    std::regex inst_match("([a-z]+) ([a-z\\-0-9]+)(?: ([a-z\\-0-9]+))?");
    std::string line;
    while(std::getline(infile, line)) {
        std::smatch match;
        if(!std::regex_match(line, match, inst_match)) {
            std::cerr << "There was an error reading in the instruction" << std::endl;
            return -1;
        }
        std::vector<std::string> instruction_pieces;
        for(size_t i = 1; i < match.size(); ++i) {
            instruction_pieces.push_back(match[i].str());
            if(verbose) {
                std::cout << match[i] << " ";
            }
        }
        if(verbose) {
            std::cout << std::endl;
        }
        program.push_back(InstructionFactory(instruction_pieces));
    }

    // Initialize the registers
    registers rs;
    rs[ip] = 0;

    while((rs[ip] >= 0) && (rs[ip] < (type_t)program.size())) {
        // Execute instruction
        if((*program[rs[ip]])(rs)) {
            // Advance to next instruction
            rs[ip] += 1;
        } else {
            std::cerr << "This shouldn't happen here!!" << std::endl;
            throw;
        }
    }

    std::cout << "mul instructions were executed " << num_mul << " times" << std::endl;

	return 0;
}
