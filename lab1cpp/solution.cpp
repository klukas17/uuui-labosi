#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <regex>
#include <map>
#include <vector>
#include <utility>

std::string alg = "", ss = "", h = "";
bool check_optimistic = false, check_consistent = false, ss_provided = false, h_provided = false;

std::string initial_state;
std::set<std::string> goal_states;
std::map<std::string, std::vector<std::pair<std::string, double>>> successor_function;
std::map<std::string, double> heuristic_function;

void read_arguments(int argc, char* argv[]) {
    int index = 1;
    while (index < argc) {
        std::string argument = argv[index];
        if (argument == "--alg") {
            alg = argv[index + 1];
            index += 2;
            continue;
        } else if (argument == "--ss") {
            ss = argv[index + 1];
            ss_provided = true;
            index += 2;
            continue;
        } else if (argument == "--h") {
            h = argv[index + 1];
            h_provided = true;
            index += 2;
            continue;
        } else if (argument == "--check-optimistic") {
            check_optimistic = true;
            index += 1;
            continue;
        } else if (argument == "--check-consistent") {
            check_consistent = true;
            index += 1;
            continue;
        }
        index++;
    }
}

void read_ss() {
    std::ifstream input(ss);
    std::string line;
    bool initial = false, goal = false;
    for (;getline(input, line);) {
        if (line[0] == '#') continue;
        if (!initial) {
            initial_state = line;
            initial = true;
            continue;
        }
        else if (!goal) {
            std::istringstream iss(line);
            std::string s;
            while (getline(iss, s, ' ')) goal_states.insert(s);
            goal = true;
            continue;
        }
        else {
            std::string left_side, right_side, state, cost;
            std::regex r1("([A-Za-zšđčćžŠĐČĆŽ]*):(.*)");
            std::regex r2(" (([A-Za-zšđčćžŠĐČĆŽ]*),([0-9]*[\\.[0-9]+]?))(.*)");
            std::cmatch match;
            std::regex_match(const_cast<char*>(line.c_str()), match, r1);
            left_side = match[1];
            right_side = match[2];
            while (right_side != "") {
                std::regex_match(const_cast<char*>(right_side.c_str()), match, r2);
                state = match[2];
                cost = match[3];
                right_side = match[4];
                successor_function[left_side].push_back({state, std::stod(cost)});
            }
        }
    }
}

void read_h() {
    std::ifstream input(h);
    std::string line;
    for (;getline(input, line);) {
        if (line[0] == '#') continue;
        std::regex r3("([A-Za-zšđčćžŠĐČĆŽ]*): ([0-9]*[\\.[0-9]+]?)");
        std::cmatch match;
        std::regex_match(const_cast<char*>(line.c_str()), match, r3);
        std::string left_side = match[1], right_side = match[2];
        heuristic_function[left_side] = std::stod(right_side);
    }
}

void print_arguments() {
    std::cout << "ARGUMENTS" << std::endl;
    std::cout << "ALG: " << alg << std::endl;
    std::cout << "SS: " << ss << std::endl;
    std::cout << "H: " << h << std::endl;
    std::cout << "CHECK-OPTIMISTIC: " << check_optimistic << std::endl;
    std::cout << "CHECK-CONSISTENT: " << check_consistent << std::endl;
    std::cout << std::endl;
}

void print_succesor_function() {
    std::cout << "SUCCESSOR FUNCTION" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    for (auto it = successor_function.begin(); it != successor_function.end(); it++) {
        auto x = it->second;
        std::cout << it->first << std::endl;
        for (auto y : x) {
            std::cout << y.first << " " << y.second << std::endl;
        }
        std::cout << "----------------------------------" << std::endl;
    }
    std::cout << std::endl;
}

void print_heuristic_function() {
    std::cout << "HEURISTIC FUNCTION" << std::endl;
    for (auto it = heuristic_function.begin(); it != heuristic_function.end(); it++) std::cout << it->first << ": " << it->second << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]){
    read_arguments(argc, argv);
    //print_arguments();
    if (ss_provided) read_ss();
    if (h_provided) read_h();
    //print_succesor_function();
    //print_heuristic_function();
}
