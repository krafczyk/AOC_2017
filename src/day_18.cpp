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

std::vector<type_t> sounds_played;
bool rcv_called = false;
bool first_rcv = true;
type_t ip = 0;

class instruction {
    public:
        virtual ~instruction() {}
        virtual void operator()(registers& r) = 0;
};

class inst_2v: public instruction {
    public:
        inst_2v(const std::string& reg, type_t v) {
            this->reg = reg;
            this->v = v;
        }
        virtual ~inst_2v() {}
        virtual void operator()(registers& r) = 0;
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
        virtual void operator()(registers& r) = 0;
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
        virtual void operator()(registers& r) = 0;
    protected:
        type_t v;
};

class inst_1r: public instruction {
    public:
        inst_1r(const std::string& reg) {
            this->reg = reg;
        }
        virtual ~inst_1r() {}
        virtual void operator()(registers& r) = 0;
    protected:
        std::string reg;
};

class sndv: public inst_1v {
    public:
        sndv(type_t v): inst_1v(v) {}
        void operator()(registers& r [[maybe_unused]]) {
            sounds_played.push_back(v);
        }
};

class sndr: public inst_1r {
    public:
        sndr(const std::string reg): inst_1r(reg) {}
        void operator()(registers& r [[maybe_unused]]) {
            sounds_played.push_back(r[reg]);
        }
};

class rcvv: public inst_1v {
    public:
        rcvv(type_t v): inst_1v(v) {}
        void operator()(registers& r [[maybe_unused]]) {
            if(v != 0) {
                rcv_called = true;
                sounds_played.push_back(sounds_played.back());
            }
        }
};

class rcvr: public inst_1r {
    public:
        rcvr(const std::string reg): inst_1r(reg) {}
        void operator()(registers& r [[maybe_unused]]) {
            if(r[reg] != 0) {
                rcv_called = true;
                sounds_played.push_back(sounds_played.back());
            }
        }
};

class setv: public inst_2v {
    public:
        setv(const std::string& reg, type_t v) : inst_2v(reg, v) {}
        void operator()(registers& r) {
            r[reg] = v;
        }
};

class setr: public inst_2r {
    public:
        setr(const std::string& regl, const std::string& regr) : inst_2r(regl, regr) {}
        void operator()(registers& r) {
            r[regl] = r[regr];
        }
};

class addv: public inst_2v {
    public:
        addv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        void operator()(registers& r) {
            r[reg] += v;
        }
};

class addr: public inst_2r {
    public:
        addr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        void operator()(registers& r) {
            r[regl] += r[regr];
        }
};

class mulv: public inst_2v {
    public:
        mulv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        void operator()(registers& r) {
            r[reg] *= v;
        }
};

class mulr: public inst_2r {
    public:
        mulr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        void operator()(registers& r) {
            r[regl] *= r[regr];
        }
};

class modv: public inst_2v {
    public:
        modv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        void operator()(registers& r) {
            r[reg] %= v;
        }
};

class modr: public inst_2r {
    public:
        modr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        void operator()(registers& r) {
            r[regl] %= r[regr];
        }
};

class jgzv: public inst_2v {
    public:
        jgzv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        void operator()(registers& r) {
            if(r[reg] > 0) {
                ip += (v-1);
            }
        }
};

class jgzr: public inst_2r {
    public:
        jgzr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        void operator()(registers& r) {
            if(r[regl] > 0) {
                ip += (r[regr]-1);
            }
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
    if(words[0] == "snd") {
        if(is_register(words[1])) {
            return (instruction*) new sndr(words[1]);
        } else {
            return (instruction*) new sndv(fetch_value<type_t>(words[1]));
        }
    } else if (words[0] == "rcv") {
        if(is_register(words[1])) {
            return (instruction*) new rcvr(words[1]);
        } else {
            return (instruction*) new rcvv(fetch_value<type_t>(words[1]));
        }
    } else if (words[0] == "set") {
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
	ArgParse::ArgParser Parser("Day 18");
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
    registers rs;
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

    size_t num = 0;
    while((ip >= 0) && (ip < (type_t)program.size())) {
        // Execute instruction
        (*program[ip])(rs);
        // Advance instruction pointer
        ip += 1;
        // Count instruction
        num += 1;
        if(rcv_called&&first_rcv) {
            std::cout << "Task 1: The first recovered frequency is: " << sounds_played.back() << std::endl;
            first_rcv = false;
            break;
        }
    }

	return 0;
}
