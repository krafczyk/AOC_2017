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

std::ostream& operator<<(std::ostream& out, const registers& rs) {
    for(auto it = rs.cbegin(); it != rs.cend(); ++it) {
        out << it->first << ":" << it->second << " ";
    }
    return out;
}

const std::string ip = "ip";

size_t num_mul = 0;

class instruction {
    public:
        instruction(const std::string& n) {
            this->n = n;
        }
        virtual ~instruction() {}
        virtual bool operator()(registers& r) = 0;
        virtual std::string str() const = 0;
        friend std::ostream& operator<<(std::ostream& out, instruction& i) {
            out << i.str();
            return out;
        }
        const std::string& name() const {
            return this->n;
        }
    private:
        std::string n;
};

class inst_2v: public instruction {
    public:
        inst_2v(const std::string& n, const std::string& reg, type_t v) : instruction(n) {
            this->reg = reg;
            this->v = v;
        }
        virtual ~inst_2v() {}
        virtual bool operator()(registers& r) = 0;
        std::string str() const {
            std::stringstream ss;
            ss << name() << " " << reg << " " << v;
            return ss.str();
        }
    protected:
        std::string reg;
        type_t v;
};

class inst_2vv: public instruction {
    public:
        inst_2vv(const std::string& n, type_t v1, type_t v2) : instruction(n) {
            this->v1 = v1;
            this->v2 = v2;
        }
        virtual ~inst_2vv() {}
        virtual bool operator()(registers& r) = 0;
        std::string str() const {
            std::stringstream ss;
            ss << name() << " " << v1 << " " << v2;
            return ss.str();
        }
    protected:
        type_t v1;
        type_t v2;
};

class inst_2r: public instruction {
    public:
        inst_2r(const std::string& n, const std::string& regl, const std::string& regr) : instruction(n) {
            this->regl = regl;
            this->regr = regr;
        }
        virtual ~inst_2r() {}
        virtual bool operator()(registers& r) = 0;
        std::string str() const {
            std::stringstream ss;
            ss << name() << " " << regl << " " << regr;
            return ss.str();
        }
    protected:
        std::string regl;
        std::string regr;
};

class inst_1v: public instruction {
    public:
        inst_1v(const std::string& n, type_t v) : instruction(n) {
            this->v = v;
        }
        virtual ~inst_1v() {}
        virtual bool operator()(registers& r) = 0;
        std::string str() const {
            std::stringstream ss;
            ss << name() << " " << v;
            return ss.str();
        }
    protected:
        type_t v;
};

class inst_1r: public instruction {
    public:
        inst_1r(const std::string& n, const std::string& reg) : instruction(n) {
            this->reg = reg;
        }
        virtual ~inst_1r() {}
        virtual bool operator()(registers& r) = 0;
        std::string str() const {
            std::stringstream ss;
            ss << name() << " " << reg;
            return ss.str();
        }
    protected:
        std::string reg;
};

class setv: public inst_2v {
    public:
        setv(const std::string& reg, type_t v) : inst_2v("set", reg, v) {}
        bool operator()(registers& r) {
            r[reg] = v;
            return true;
        }
};

class setr: public inst_2r {
    public:
        setr(const std::string& regl, const std::string& regr) : inst_2r("set", regl, regr) {}
        bool operator()(registers& r) {
            r[regl] = r[regr];
            return true;
        }
};

class addv: public inst_2v {
    public:
        addv(const std::string& reg, type_t v): inst_2v("add", reg, v) {}
        bool operator()(registers& r) {
            r[reg] += v;
            return true;
        }
};

class addr: public inst_2r {
    public:
        addr(const std::string& regl, const std::string& regr): inst_2r("add", regl, regr) {}
        bool operator()(registers& r) {
            r[regl] += r[regr];
            return true;
        }
};

class subv: public inst_2v {
    public:
        subv(const std::string& reg, type_t v): inst_2v("sub", reg, v) {}
        bool operator()(registers& r) {
            r[reg] -= v;
            return true;
        }
};

class subr: public inst_2r {
    public:
        subr(const std::string& regl, const std::string& regr): inst_2r("sub", regl, regr) {}
        bool operator()(registers& r) {
            r[regl] -= r[regr];
            return true;
        }
};

class mulv: public inst_2v {
    public:
        mulv(const std::string& reg, type_t v): inst_2v("mul", reg, v) {}
        bool operator()(registers& r) {
            num_mul += 1;
            r[reg] *= v;
            return true;
        }
};

class mulr: public inst_2r {
    public:
        mulr(const std::string& regl, const std::string& regr): inst_2r("mul", regl, regr) {}
        bool operator()(registers& r) {
            num_mul += 1;
            r[regl] *= r[regr];
            return true;
        }
};

class modv: public inst_2v {
    public:
        modv(const std::string& reg, type_t v): inst_2v("mod", reg, v) {}
        bool operator()(registers& r) {
            r[reg] %= v;
            return true;
        }
};

class modr: public inst_2r {
    public:
        modr(const std::string& regl, const std::string& regr): inst_2r("mod", regl, regr) {}
        bool operator()(registers& r) {
            r[regl] %= r[regr];
            return true;
        }
};

class jgzv: public inst_2v {
    public:
        jgzv(const std::string& reg, type_t v): inst_2v("jgz", reg, v) {}
        bool operator()(registers& r) {
            if(r[reg] > 0) {
                r[ip] += (v-1);
            }
            return true;
        }
};

class jgzr: public inst_2r {
    public:
        jgzr(const std::string& regl, const std::string& regr): inst_2r("jgz", regl, regr) {}
        bool operator()(registers& r) {
            if(r[regl] > 0) {
                r[ip] += (r[regr]-1);
            }
            return true;
        }
};

class jnzv: public inst_2v {
    public:
        jnzv(const std::string& reg, type_t v): inst_2v("jnz", reg, v) {}
        bool operator()(registers& r) {
            if(r[reg] != 0) {
                r[ip] += (v-1);
            }
            return true;
        }
};

class jnzvv: public inst_2vv {
    public:
        jnzvv(type_t v1, type_t v2): inst_2vv("jnz", v1, v2) {}
        bool operator()(registers& r) {
            if(v1 != 0) {
                r[ip] += (v2-1);
            }
            return true;
        }
};

class jnzr: public inst_2r {
    public:
        jnzr(const std::string& regl, const std::string& regr): inst_2r("jnz", regl, regr) {}
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
        if(is_register(words[1])) {
            if(is_register(words[2])) {
                return (instruction*) new jnzr(words[1], words[2]);
            } else {
                return (instruction*) new jnzv(words[1], fetch_value<type_t>(words[2]));
            }
        } else {
            if(is_register(words[2])) {
                std::cerr << "register second word is not supported when jnz accepts a value as the first value" << std::endl;
                throw;
            } else {
                return (instruction*) new jnzvv(fetch_value<type_t>(words[1]), fetch_value<type_t>(words[2]));
            }
        }
    } else {
        std::cerr << "Encountered an unknown command!" << std::endl;
        throw;
    }
    std::cerr << "We shouldn't get to here!" << std::endl;
    throw;
}

std::vector<type_t> gen_primes(type_t N) {
    std::vector<type_t> primes;
    primes.push_back(2);
    for(int f = 3; f <= N; ++f) {
        bool factor_found = false;
        for(size_t i = 0; i < primes.size(); ++i) {
            if(f%primes[i] == 0) {
                factor_found = true;
                break;
            }
        }
        if(!factor_found) {
            primes.push_back(f);
        }
    }
    return primes;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose1 = false;
	bool verbose2 = false;
    size_t num1 = 0;
    bool num1_defined = false;
    size_t num2 = 0;
    bool num2_defined = false;
	ArgParse::ArgParser Parser("Day 23");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v1/--verbose1", "Print Verbose output", &verbose1);
    Parser.AddArgument("-n1/--num1", "number of instructions to execute", &num1, ArgParse::Argument::Optional, &num1_defined);
	Parser.AddArgument("-v2/--verbose2", "Print Verbose output", &verbose2);
    Parser.AddArgument("-n2/--num2", "number of instructions to execute", &num2, ArgParse::Argument::Optional, &num2_defined);

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
        }
        program.push_back(InstructionFactory(instruction_pieces));
    }

    // Initialize the registers
    registers rs;
    rs[ip] = 0;

    std::cout << "Initial registers: " << rs << std::endl;
    size_t n = 0;
    while((rs[ip] >= 0) && (rs[ip] < (type_t)program.size()) && ((!num1_defined)||(n < num1))) {
        if(verbose1) {
            std::cout << rs[ip] << ": " << (*program[rs[ip]]) << " -- ";
        }
        // Execute instruction
        if((*program[rs[ip]])(rs)) {
            // Advance to next instruction
            rs[ip] += 1;
        } else {
            std::cerr << "This shouldn't happen here!!" << std::endl;
            throw;
        }
        if(verbose1) {
            std::cout << rs << std::endl;
        }
        n += 1;
    }

    std::cout << "Task 1: mul instructions were executed " << num_mul << " times" << std::endl;

    // Initialize the registers

    rs.clear();
    rs[ip] = 0;
    rs["a"] = 1;

    n = 0;
    while((rs[ip] >= 0) && (rs[ip] < (type_t)program.size()) && (n < 8)) {
        if(verbose2) {
            std::cout << rs[ip] << ": " << (*program[rs[ip]]) << " -- ";
        }
        // Execute instruction
        if((*program[rs[ip]])(rs)) {
            // Advance to next instruction
            rs[ip] += 1;
        } else {
            std::cerr << "This shouldn't happen here!!" << std::endl;
            throw;
        }
        if(verbose2) {
            std::cout << rs << std::endl;
        }
        n += 1;
    }

    // Build prime list
    type_t b = rs["b"];
    type_t c = rs["c"];
    if(verbose2) {
        std::cout << "b: " << b << " c: " << c << std::endl;
    }
    std::vector<type_t> primes = gen_primes(std::max(b,c));

    type_t h = 0;
    for(type_t v = std::min(b,c); v <= std::max(b,c); ++v) {
        if(hasElement(primes, v)) {
            h += 1;
        }
    }

    std::cout << "Task 2: The h register is: " << h << std::endl;

	return 0;
}
