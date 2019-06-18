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
#include <queue>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef long type_t;
typedef std::unordered_map<std::string,type_t> registers;
typedef std::vector<type_t> messages;

std::vector<type_t> sounds_played;
const std::string ip = "ip";

class instruction {
    public:
        virtual ~instruction() {}
        virtual bool operator()(registers& r, messages& snd, messages& rcv) = 0;
};

class inst_2v: public instruction {
    public:
        inst_2v(const std::string& reg, type_t v) {
            this->reg = reg;
            this->v = v;
        }
        virtual ~inst_2v() {}
        virtual bool operator()(registers& r, messages& snd, messages& rcv) = 0;
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
        virtual bool operator()(registers& r, messages& snd, messages& rcv) = 0;
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
        virtual bool operator()(registers& r, messages& snd, messages& rcv) = 0;
    protected:
        type_t v;
};

class inst_1r: public instruction {
    public:
        inst_1r(const std::string& reg) {
            this->reg = reg;
        }
        virtual ~inst_1r() {}
        virtual bool operator()(registers& r, messages& snd, messages& rcv) = 0;
    protected:
        std::string reg;
};

class sndv: public inst_1v {
    public:
        sndv(type_t v): inst_1v(v) {}
        bool operator()(registers& r [[maybe_unused]], messages& snd, messages& rcv [[maybe_unused]]) {
            snd.push_back(v);
            return true;
        }
};

class sndr: public inst_1r {
    public:
        sndr(const std::string reg): inst_1r(reg) {}
        bool operator()(registers& r, messages& snd, messages& rcv [[maybe_unused]]) {
            snd.push_back(r[reg]);
            return true;
        }
};

//class rcvv: public inst_1v {
//    public:
//        rcvv(type_t v): inst_1v(v) {}
//        void operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv) {
//            if(v != 0) {
//                rcv_called = true;
//                sounds_played.push_back(sounds_played.back());
//            }
//            return true;
//        }
//};

class rcvr: public inst_1r {
    public:
        rcvr(const std::string reg): inst_1r(reg) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv) {
            if(rcv.size() != 0) {
                r[reg] = rcv.front();
                rcv.erase(rcv.begin());
                return true;
            } else {
                return false;
            }
        }
};

class setv: public inst_2v {
    public:
        setv(const std::string& reg, type_t v) : inst_2v(reg, v) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[reg] = v;
            return true;
        }
};

class setr: public inst_2r {
    public:
        setr(const std::string& regl, const std::string& regr) : inst_2r(regl, regr) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[regl] = r[regr];
            return true;
        }
};

class addv: public inst_2v {
    public:
        addv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[reg] += v;
            return true;
        }
};

class addr: public inst_2r {
    public:
        addr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[regl] += r[regr];
            return true;
        }
};

class mulv: public inst_2v {
    public:
        mulv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[reg] *= v;
            return true;
        }
};

class mulr: public inst_2r {
    public:
        mulr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[regl] *= r[regr];
            return true;
        }
};

class modv: public inst_2v {
    public:
        modv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[reg] %= v;
            return true;
        }
};

class modr: public inst_2r {
    public:
        modr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            r[regl] %= r[regr];
            return true;
        }
};

class jgzv: public inst_2v {
    public:
        jgzv(const std::string& reg, type_t v): inst_2v(reg, v) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            if(r[reg] > 0) {
                r[ip] += (v-1);
            }
            return true;
        }
};

class jgzr: public inst_2r {
    public:
        jgzr(const std::string& regl, const std::string& regr): inst_2r(regl, regr) {}
        bool operator()(registers& r, messages& snd [[maybe_unused]], messages& rcv [[maybe_unused]]) {
            if(r[regl] > 0) {
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
            std::cerr << "No rcvv instruction!!" << std::endl;
            throw;
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

    // First simulation
    messages single;
    // Initialize the registers
    registers single_rs;
    single_rs["p"] = 0;
    single_rs[ip] = 0;

    while((single_rs[ip] >= 0) && (single_rs[ip] < (type_t)program.size())) {
        // Execute instruction
        size_t before_size = single.size();
        if((*program[single_rs[ip]])(single_rs, single, single)) {
            // Advance to next instruction
            single_rs[ip] += 1;
            if(before_size > single.size()) {
                // A value was received
                std::cout << "Task 1: The first recovered frequency is: " << single.back() << std::endl;
                break;
            }
        } else {
            std::cerr << "This shouldn't happen here!!" << std::endl;
            throw;
        }
    }

    // Second simulation
    std::map<int,messages> message = { {0, messages()}, {1, messages()}};
    std::map<int,registers> regs = { {0, registers()}, {1, registers()}};
    std::map<int,bool> waiting_for_values = {{0, false}, {1, false}};
    regs[0]["p"] = 0;
    regs[0][ip] = 0;
    regs[1]["p"] = 1;
    regs[1][ip] = 0;

    int current_program = 0;
    int idle_program = (current_program+1)%2;
    size_t num_vals_sent = 0;
    while(true) {
        // Execute instruction
        size_t before_size = message[idle_program].size();
        if((*program[regs[current_program][ip]])(regs[current_program], message[idle_program], message[current_program])) {
            // Advance to next instruction
            regs[current_program][ip] += 1;
            if(message[idle_program].size() > before_size) {
                if(current_program == 1) {
                    num_vals_sent += 1;
                }
            }
            // Reset the value waiting
            if(waiting_for_values[current_program]) {
                waiting_for_values[current_program] = false;
            }
        } else {
            // We don't have a value to receive.
            if(waiting_for_values[current_program]) {
                // We've reached a deadlock
                break;
            } else {
                // Indicate we're not waiting, change programs.
                waiting_for_values[current_program] = true;
                if(verbose) {
                    std::cout << "Switching from " << current_program;
                }
                current_program = (current_program+1)%2;
                if(verbose) {
                    std:: cout << " to " << current_program << std::endl;
                }
                idle_program = (idle_program+1)%2;
            }
        }
    }

    std::cout << "Task 2: Program 1 sent " << num_vals_sent << " values" << std::endl;

	return 0;
}
