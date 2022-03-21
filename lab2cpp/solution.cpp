#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

#include "Clause.h"

std::string mode;
std::string clauses_path;
std::string commands_path;
std::vector<Clause*> clauses;

void read_arguments(int argc, char* argv[]) {
    mode = argv[1];
    clauses_path = argv[2];
    if (argc == 4) commands_path = argv[3];
}

void read_clauses() {
    std::ifstream input(clauses_path);
    std::string line;
    for (;getline(input, line);) {

        if (line[0] == '#') continue;

        std::transform(line.begin(), line.end(), line.begin(), [](char c){ return std::tolower(c); });

        std::istringstream iss(line);
        std::string s;
        bool skip = false;
        Clause* new_clause = new Clause();
        while (getline(iss, s, ' ')) {
            if (skip) {
                skip = false;
                continue;
            }
            skip = true;
            if (s[0] != '~')
                new_clause->literals.insert(s);
            else
                new_clause->literals_negated.insert(s.substr(1, s.size()));
        }
        clauses.push_back(new_clause);
    }

    if (mode == "resolution") {
        Clause* last_clause = clauses.rbegin()[0];
        std::set<std::string> tmp = last_clause->literals_negated;
        last_clause->literals_negated = last_clause->literals;
        last_clause->literals = tmp;
    }
}

void read_commands() {

}

void resolution() {
    read_clauses();
    std::cout << "done" << std::endl;
}

void cooking() {

}

int main(int argc, char* argv[]){
    if (argc != 3 && argc != 4) exit(1);
    read_arguments(argc, argv);
    if (mode == "resolution") resolution();
    else if (mode == "cooking") cooking();
}
