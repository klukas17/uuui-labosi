#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <regex>
#include <map>
#include <vector>
#include <utility>
#include <queue>
#include <iomanip>
#include <float.h>
#include "Node.h"

std::string alg = "", ss = "", h = "";
bool check_optimistic = false, check_consistent = false, ss_provided = false, h_provided = false;

std::string initial_state;
std::set<std::string> goal_states;
std::map<std::string, std::vector<std::pair<std::string, double>>> successor_function;
std::map<std::string, double> heuristic_function;

std::map<std::string, std::map<std::string, double>> Dijkstra_map;

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
            std::regex r1("([A-Za-zšđčćžŠĐČĆŽ0-9_]*):(.*)");
            std::regex r2(" (([A-Za-zšđčćžŠĐČĆŽ0-9_]*),([0-9]*[\\.[0-9]+]?))(.*)");
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
            std::sort(successor_function[left_side].begin(), successor_function[left_side].end(),
                 [] (std::pair<std::string, double> i1, std::pair<std::string, double> i2) {return (i1.first < i2.first);});
        }
    }
}

void read_h() {
    std::ifstream input(h);
    std::string line;
    for (;getline(input, line);) {
        if (line[0] == '#') continue;
        std::regex r3("([A-Za-zšđčćžŠĐČĆŽ0-9_]*): ([0-9]*[\\.[0-9]+]?)");
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

void print_results(bool found, std::set<std::string> visited, Node* result) {
    if (found) {
        std::cout << "[FOUND_SOLUTION]: yes" << std::endl;
        std::cout << "[STATES_VISITED]: " << visited.size() << std::endl;
        std::cout << "[PATH_LENGTH]: " << result->path_length() << std::endl;
        std::cout << "[TOTAL_COST]: " << result->total_cost() << std::endl;
        std::cout << "[PATH]:";
        result->print_path();
        std::cout << std::endl;
    }

    else {
        std::cout << "[FOUND_SOLUTION]: no" << std::endl;
    }
}

void bfs() {

    std::queue<Node*> open;
    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;

    open.push(new Node(initial_state, nullptr, 0));
    while (open.size() > 0) {
        Node* n = open.front();
        open.pop();
        visited.insert(n->state);
        if (goal_states.find(n->state) != goal_states.end()) {
            found = true;
            result = n;
            break;
        }
        for (std::pair<std::string, double> m : successor_function[n->state]) {
            if (visited.find(m.first) == visited.end()) open.push(new Node(m.first, n, n->cost + m.second));
        }
    }

    std::cout << "# BFS" << std::endl;
    print_results(found, visited, result);
}

void ucs() {
    auto comparator = [] (Node* a, Node* b) {
        if (a->cost != b->cost) return (a->cost > b->cost);
        else return (a->state > b->state);
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(comparator)> open(comparator);

    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;

    open.push(new Node(initial_state, nullptr, 0));
    while (open.size() > 0) {
        Node* n = open.top();
        open.pop();
        visited.insert(n->state);
        if (goal_states.find(n->state) != goal_states.end()) {
            found = true;
            result = n;
            break;
        }
        for (std::pair<std::string, double> m : successor_function[n->state]) {
            if (visited.find(m.first) == visited.end()) open.push(new Node(m.first, n, n->cost + m.second));
        }
    }

    std::cout << "# UCS" << std::endl;
    print_results(found, visited, result);
}

void astar() {
    auto comparator = [] (Node* a, Node* b) {
        if (a->f != b->f) return (a->f > b->f);
        else return (a->f > b->f);
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(comparator)> open(comparator);

    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;
    std::set<Node*> closed;
    std::map<std::string, double> front_map;

    for (auto it = heuristic_function.begin(); it != heuristic_function.end(); it++)
        front_map[it->first] = DBL_MAX;

    front_map[initial_state] = 0;
    open.push(new Node(initial_state, nullptr, 0));

    while (open.size() > 0) {
        Node* n = open.top();
        open.pop();
        visited.insert(n->state);
        if (goal_states.find(n->state) != goal_states.end()) {
            found = true;
            result = n;
            break;
        }
        closed.insert(n);
        for (std::pair<std::string, double> u : successor_function[n->state]) {
            Node* m = new Node(u.first, n, n->cost + u.second);
            bool flag = false;
            for (Node* m_ : closed) {
                if (m_->state == m->state) {
                    if (m_->cost < m->cost) {
                        flag = true;
                        m->~Node();
                        break;
                    }
                    else {
                        closed.erase(m_);
                    }
                }
            }
            if (flag) continue;
            if (front_map[m->state] < m->cost) {
                flag = true;
                m->~Node();
            }
            front_map[m->state] = m->cost;
            m->f = m->cost + heuristic_function[m->state];
            open.push(m);
        }
    }

    std::cout << "# A-STAR "<< h << std::endl;
    print_results(found, visited, result);
}

void Dijkstra() {
    std::map<std::string, double> dist;
    std::map<std::string, std::string> prev;

    auto comparator = [&dist] (std::string a, std::string b) {
        return (dist[a] < dist[b]);
    };
    std::priority_queue<std::string, std::vector<std::string>, decltype(comparator)> Q(comparator);

    for (auto it = heuristic_function.begin(); it != heuristic_function.end(); it++) {
        dist[it.first] = DBL_MAX;
        prev[it.first] = nullptr;
    }
}

int main(int argc, char* argv[]){
    read_arguments(argc, argv);
    //print_arguments();
    if (ss_provided) read_ss();
    if (h_provided) read_h();
    //print_succesor_function();
    //print_heuristic_function();
    std::cout << std::setprecision(1) << std::fixed;
    if (alg == "bfs") bfs();
    else if (alg == "ucs") ucs();
    else if (alg == "astar") astar();
    if (check_consistent || check_optimistic) Dijkstra();
}
