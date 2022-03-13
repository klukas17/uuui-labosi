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
 *      improve input reading
 *      improve check-optimistic
 *      run autograder
 */

std::string alg = "", ss = "", h = "";
bool check_optimistic = false, check_consistent = false, ss_provided = false, h_provided = false;

std::string initial_state;
std::set<std::string> states;
std::set<std::string> goal_states;
std::map<std::string, std::vector<std::pair<std::string, double>>> successor_function;
std::map<std::string, double> heuristic_function;

std::map<std::string, double> distance;
std::map<std::string, double> distances;

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
    int l = 2;
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
            states.insert(left_side);
            while (right_side != "") {
                std::regex_match(const_cast<char*>(right_side.c_str()), match, r2);
                state = match[2];
                cost = match[3];
                right_side = match[4];
                successor_function[left_side].push_back({state, std::stod(cost)});
            }
            std::sort(successor_function[left_side].begin(), successor_function[left_side].end(),
                 [] (std::pair<std::string, double> i1, std::pair<std::string, double> i2) {return (i1.first < i2.first);});
            l++;
            if (l % 1000 == 0) std::cout << l << std::endl;
        }
    }
}

void read_h() {
    std::ifstream input(h);
    std::string line;
    int l = 0;
    for (;getline(input, line);) {
        if (line[0] == '#') continue;
        std::regex r3("([A-Za-zšđčćžŠĐČĆŽ0-9_]*): ([0-9]*[\\.[0-9]+]?)");
        std::cmatch match;
        std::regex_match(const_cast<char*>(line.c_str()), match, r3);
        std::string left_side = match[1], right_side = match[2];
        heuristic_function[left_side] = std::stod(right_side);
        l++;
        if (l % 1000 == 0) std::cout << l << std::endl;
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
    std::set<Node*> closed;
    std::map<std::string, double> front_map;

    for (auto x : states)
        front_map[x] = DBL_MAX;

    front_map[initial_state] = 0;

    Node* start_node = new Node(initial_state, nullptr, 0);
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
        closed.insert(n);
        for (std::pair<std::string, double> u : successor_function[n->state]) {
            Node* m = new Node(u.first, n, n->cost + u.second);
            all_nodes.push_back(m);
            bool flag = false;
            for (Node* m_ : closed) {
                if (m_->state == m->state) {
                    if (m_->cost < m->cost) {
                        flag = true;
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
            }
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

void Dijkstra() {

    std::map<std::string, std::map<std::string, double>> Dijkstra_map;

    for (auto it = goal_states.begin(); it != goal_states.end(); it++) {
        std::string goal_state = *it;
        std::map<std::string, double> dist;
        std::map<std::string, bool> finished;

        auto comparator = [] (std::pair<std::string, double> a, std::pair<std::string, double> b) {
            return (a.second > b.second);
        };
        std::priority_queue<std::pair<std::string, double>, std::vector<std::pair<std::string, double>>, decltype(comparator)> Q(comparator);

        for (auto x : states) {
            dist[x] = DBL_MAX;
        }

        dist[goal_state] = 0;
        Q.push({goal_state, 0});

        while (Q.size() > 0) {
            auto u = Q.top();
            Q.pop();
            if (finished[u.first] && dist[u.first] < u.second) continue;
            finished[u.first] = true;
            dist[u.first] = u.second;
            for (auto neighbor : successor_function[u.first]) {
                auto v = neighbor.first;
                auto d = neighbor.second;
                double alt = u.second + d;
                if (alt < dist[v]) {
                    Q.push({v, alt});
                }
            }
        }

        Dijkstra_map[goal_state] = dist;
    }

    for (auto x : states) {
        distances[x] = DBL_MAX;
    }

    for (auto it = Dijkstra_map.begin(); it != Dijkstra_map.end(); it++) {
        auto x = (*it).second;
        for (auto pair : x) {
            auto y = pair.first;
            auto z = pair.second;
            if (distances[y] > z) {
                distances[y] = z;
            }
        }
    }

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
