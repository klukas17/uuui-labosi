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

/*
 * TODO:
 *      find all mistakes with autograder
 *      improve check-optimistic
 */

std::string alg = "", ss = "", h = "";
bool check_optimistic = false, check_consistent = false, ss_provided = false, h_provided = false;

std::string initial_state;
std::set<std::string> states;
std::set<std::string> goal_states;
std::map<std::string, std::vector<std::pair<std::string, double>>> successor_function;
std::map<std::string, double> heuristic_function;

std::map<std::string, double> distance;

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
            while (getline(iss, s, ' '))
                goal_states.insert(s);
            goal = true;
            continue;
        }

        else {
            std::string left_side, right_side;
            int pos = line.find(':');
            left_side = line.substr(0, pos);
            states.insert(left_side);
            right_side = line.substr(pos + 1, line.size() - pos);
            if (right_side.size() > 0)
                right_side = right_side.substr(1, right_side.size());
            std::stringstream stream(right_side);
            std::string segment;
            while (getline(stream, segment, ' ')) {
                std::string state, cost;
                int breaking_point = segment.find(',');
                state = segment.substr(0, breaking_point);
                cost = segment.substr(breaking_point + 1, segment.size());
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
        std::string left_side, right_side;
        int pos = line.find(':');
        left_side = line.substr(0, pos);
        right_side = line.substr(pos + 2, line.size() - pos);
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

    std::vector<Node*> all_nodes;

    std::queue<Node*> open;
    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;

    Node* start_node = new Node(initial_state, nullptr, 0);
    all_nodes.push_back(start_node);
    open.push(start_node);

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
            if (visited.find(m.first) == visited.end()) {
                Node* new_node = new Node(m.first, n, n->cost + m.second);
                all_nodes.push_back(new_node);
                open.push(new_node);
            }
        }
    }

    std::cout << "# BFS" << std::endl;
    print_results(found, visited, result);

    for (auto n : all_nodes)
        n->~Node();
}

double ucs(bool print, std::string start) {

    std::vector<Node*> all_nodes;

    auto comparator = [] (Node* a, Node* b) {
        if (a->cost != b->cost) return (a->cost > b->cost);
        else return (a->state > b->state);
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(comparator)> open(comparator);

    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;

    Node* start_node = new Node(start, nullptr, 0);
    all_nodes.push_back(start_node);
    open.push(start_node);

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
            if (visited.find(m.first) == visited.end()) {
                Node* new_node = new Node(m.first, n, n->cost + m.second);
                all_nodes.push_back(new_node);
                open.push(new_node);
            }
        }
    }

    double ret_val = result->cost;

    if (print) {
        std::cout << "# UCS" << std::endl;
        print_results(found, visited, result);
    }

    for (auto n : all_nodes)
        n->~Node();

    return ret_val;
}

/*
void astar() {

    std::vector<Node*> all_nodes;

    auto comparator = [] (Node* a, Node* b) {
        if (a->f != b->f) return (a->f > b->f);
        else return (a->f > b->f);
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(comparator)> open(comparator);

    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;
    //std::set<Node*> closed;
    std::map<Node*, bool> closed;
    std::map<std::string, double> front_map;

    for (auto x : states)
        front_map[x] = DBL_MAX;

    front_map[initial_state] = 0;

    Node* start_node = new Node(initial_state, nullptr, 0);
    all_nodes.push_back(start_node);
    open.push(start_node);

    int l = 0;

    while (open.size() > 0) {
        Node* n = open.top();
        open.pop();
        visited.insert(n->state);
        //if ((++l) % 100 == 0) std::cout << l << std::endl;
        if (goal_states.find(n->state) != goal_states.end()) {
            found = true;
            result = n;
            break;
        }
        //closed.insert(n);
        closed[n] = true;
        for (std::pair<std::string, double> u : successor_function[n->state]) {
            Node* m = new Node(u.first, n, n->cost + u.second);
            all_nodes.push_back(m);
            bool flag = false;
            std::vector<Node*> to_delete;
            //for (Node* m_ : closed) {
            std::cout << closed.size() << std::endl;
            for (auto pair : closed) {
                Node* m_ = pair.first;
                if (m_->state == m->state) {
                    if (m_->cost < m->cost) {
                        flag = true;
                        break;
                    }
                    else {
                        to_delete.push_back(m_);
                    }
                }
            }
            for (auto v : to_delete)
                closed.erase(v);
            if (flag) continue;
            if (front_map[m->state] < m->cost) {
                flag = true;
            }
            if (flag) continue;
            front_map[m->state] = m->cost;
            m->f = m->cost + heuristic_function[m->state];
            open.push(m);
        }
    }

    std::cout << "# A-STAR "<< h << std::endl;
    print_results(found, visited, result);

    for (auto n : all_nodes)
        n->~Node();
}
 */

/*
void astar() {

    std::vector<Node*> all_nodes;

    auto comparator = [] (Node* a, Node* b) {
        if (a->f != b->f) return (a->f > b->f);
        else return (a->f > b->f);
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(comparator)> open(comparator);

    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;
    //std::set<Node*> closed;
    //std::map<Node*, bool> closed;
    std::map<std::string, std::map<Node*, bool>> closed;
    std::map<std::string, double> front_map;

    for (auto x : states)
        front_map[x] = DBL_MAX;

    front_map[initial_state] = 0;

    Node* start_node = new Node(initial_state, nullptr, 0);
    all_nodes.push_back(start_node);
    open.push(start_node);

    int l = 0;

    while (open.size() > 0) {
        Node* n = open.top();
        open.pop();
        visited.insert(n->state);
        if ((++l) % 100 == 0) std::cout << l << std::endl;
        if (goal_states.find(n->state) != goal_states.end()) {
            found = true;
            result = n;
            break;
        }
        //closed.insert(n);
        for (std::pair<std::string, double> u : successor_function[n->state]) {
            Node* m = new Node(u.first, n, n->cost + u.second);
            all_nodes.push_back(m);
            bool flag = false;
            std::vector<Node*> to_delete;
            //for (Node* m_ : closed) {
            for (auto pair : closed[m->state]) {
                Node* m_ = pair.first;
                if (m_->state == m->state) {
                    if (m_->cost < m->cost) {
                        flag = true;
                        break;
                    }
                    else {
                        to_delete.push_back(m_);
                    }
                }
            }
            for (auto v : to_delete)
                closed[m->state].erase(v);
            if (flag) continue;
            if (front_map[m->state] < m->cost)
                continue;
            front_map[m->state] = m->cost;
            m->f = m->cost + heuristic_function[m->state];
            open.push(m);
        }
    }

    std::cout << "# A-STAR "<< h << std::endl;
    print_results(found, visited, result);

    for (auto n : all_nodes)
        n->~Node();
}
 */

void astar() {
    std::vector<Node*> all_nodes;

    auto comparator = [] (Node* a, Node* b) {
        if (a->f != b->f) return (a->f > b->f);
        else return (a->state > b->state);
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(comparator)> open(comparator);

    std::map<std::string, double> front_map;

    for (auto x : states)
        front_map[x] = DBL_MAX;

    front_map[initial_state] = 0;

    std::set<std::string> visited;
    bool found = false;
    Node* result = nullptr;

    std::map<std::string, std::map<Node*, bool>> closed;

    Node* start_node = new Node(initial_state, nullptr, 0);
    start_node->f = heuristic_function[initial_state];
    all_nodes.push_back(start_node);
    open.push(start_node);

    while (open.size() > 0) {
        Node *n = open.top();
        open.pop();

        if (front_map[n->state] < n->cost) continue;

        visited.insert(n->state);

        if (goal_states.find(n->state) != goal_states.end()) {
            found = true;
            result = n;
            break;
        }

        closed[n->state][n] = true;

        for (std::pair<std::string, double> u : successor_function[n->state]) {
            Node *m = new Node(u.first, n, n->cost + u.second);
            all_nodes.push_back(m);
            bool flag = false;
            for (auto pair: closed[m->state]) {
                Node *m_ = pair.first;
                if (!closed[m_->state][m_]) continue;
                if (m_->state == m->state) {
                    if (m_->cost < m->cost) {
                        flag = true;
                        break;
                    } else {
                        closed[m_->state][m_] = false;
                    }
                }
            }
            if (flag) continue;
            if (front_map[m->state] < m->cost) continue;
            front_map[m->state] = m->cost;
            m->f = m->cost + heuristic_function[m->state];
            open.push(m);
        }
    }

    std::cout << "# A-STAR "<< h << std::endl;
    print_results(found, visited, result);

    for (auto n : all_nodes)
        n->~Node();
}

void calculate_distance_ucs() {
    for (auto state : states)
        distance[state] = ucs(false, state);
}

void check_heuristic_optimistic() {
    std::cout << "# HEURISTIC-OPTIMISTIC "<< h << std::endl;
    bool optimistic = true;

    for (auto pair : heuristic_function) {
        auto state = pair.first;
        auto heuristic_cost = pair.second;
        auto real_cost = distance[state];
        if (heuristic_cost <= real_cost)
            std::cout << "[CONDITION]: [OK] h(" << state << ") <= h*: " << heuristic_cost << " <= " << real_cost << std::endl;
        else {
            std::cout << "[CONDITION]: [ERR] h(" << state << ") <= h*: " << heuristic_cost << " <= " << real_cost << std::endl;
            optimistic = false;
        }
    }

    if (optimistic)
        std::cout << "[CONCLUSION]: Heuristic is optimistic." << std::endl;
    else
        std::cout << "[CONCLUSION]: Heuristic is not optimistic." << std::endl;
}

void check_heuristic_consistent() {
    std::cout << "# HEURISTIC-CONSISTENT "<< h << std::endl;
    bool consistent = true;

    for (auto pair : successor_function) {
        auto state = pair.first;
        auto neighbours = pair.second;
        for (auto m : neighbours) {
            auto n = m.first;
            auto c = m.second;
            auto h1 = heuristic_function[state];
            auto h2 = heuristic_function[n];
            if (h1 <= h2 + c)
                std::cout << "[CONDITION]: [OK] h(" << state << ") <= h(" << n << ") + c: " << h1 << " <= " << h2 << " + " << c << std::endl;
            else {
                std::cout << "[CONDITION]: [ERR] h(" << state << ") <= h(" << n << ") + c: " << h1 << " <= " << h2 << " + " << c << std::endl;
                consistent = false;
            }
        }
    }

    if (consistent)
        std::cout << "[CONCLUSION]: Heuristic is consistent." << std::endl;
    else
        std::cout << "[CONCLUSION]: Heuristic is not consistent." << std::endl;
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
    else if (alg == "ucs") ucs(true, initial_state);
    else if (alg == "astar") astar();
    if (check_optimistic){
        calculate_distance_ucs();
        check_heuristic_optimistic();
    };
    if (check_consistent)
        check_heuristic_consistent();
}
