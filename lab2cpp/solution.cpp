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
std::set<Clause*> clauses;
std::set<Clause*> set_of_support;
std::set<std::pair<std::set<std::string>, std::set<std::string>>> hashed_clauses;

int curr_index = 0;

void read_arguments(int argc, char* argv[]) {
    mode = argv[1];
    clauses_path = argv[2];
    if (argc == 4) commands_path = argv[3];
}

void read_clauses() {
    std::ifstream input(clauses_path);
    std::string line;
    Clause* last_clause = nullptr;
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

        std::set<std::string> i;
        std::set_intersection(new_clause->literals.begin(), new_clause->literals.end(), new_clause->literals_negated.begin(), new_clause->literals_negated.end(), std::inserter(i, i.begin()));

        if (i.size() > 0) {
            new_clause->~Clause();
        }

        else {
            curr_index++;
            new_clause->index = curr_index;
            if (last_clause)
                clauses.insert(last_clause);
            last_clause = new_clause;
        }
    }

    if (mode == "resolution") {
        std::set<std::string> tmp = last_clause->literals_negated;
        last_clause->literals_negated = last_clause->literals;
        last_clause->literals = tmp;
        set_of_support.insert(last_clause);
    }

    else if (mode == "cooking") {
        clauses.insert(last_clause);
    }
}

void read_commands() {

}

void resolution_print_results(bool result) {

}

void resolution() {
    read_clauses();

    unsigned long set_of_support_size = set_of_support.size();

    int time=0;

    while(true) {

        std::cout << ++time << std::endl;

        std::set<Clause*> testing_set;
        std::set_union(clauses.begin(), clauses.end(), set_of_support.begin(), set_of_support.end(), std::inserter(testing_set, testing_set.begin()));

        for (auto c1 : set_of_support) {
            std::set<Clause*> d;
            std::set_union(set_of_support.begin(), set_of_support.end(), clauses.begin(), clauses.end(), std::inserter(d, d.begin()));
            for (auto c2 : d) {

                if (c1 == c2) continue;

                std::set<Clause*> resolvents;
                std::set<std::string> s1, s2;
                std::set_intersection(c1->literals.begin(), c1->literals.end(), c2->literals_negated.begin(), c2->literals_negated.end(), std::inserter(s1, s1.begin()));
                std::set_intersection(c1->literals_negated.begin(), c1->literals_negated.end(), c2->literals.begin(), c2->literals.end(), std::inserter(s2, s2.begin()));

                std::set<std::string> union_of_intersects;
                std::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(union_of_intersects, union_of_intersects.begin()));

                for (auto x : union_of_intersects) {
                    Clause* new_clause = new Clause();
                    for (auto y : c1->literals) if (y != x) new_clause->literals.insert(y);
                    for (auto y : c2->literals) if (y != x) new_clause->literals.insert(y);
                    for (auto y : c1->literals_negated) if (y != x) new_clause->literals_negated.insert(y);
                    for (auto y : c2->literals_negated) if (y != x) new_clause->literals_negated.insert(y);

                    new_clause->parent_clausules.push_back(c1);
                    new_clause->parent_clausules.push_back(c2);

                    std::set<std::string> i;
                    std::set_intersection(new_clause->literals.begin(), new_clause->literals.end(), new_clause->literals_negated.begin(), new_clause->literals_negated.end(), std::inserter(i, i.begin()));

                    if (i.size() > 0) {
                        new_clause->~Clause();
                    }
                    else {

                        bool flag = true;

                        for (auto clause : testing_set) {
                            std::set<std::string> testing_set1, testing_set2;
                            std::set_union(clause->literals.begin(), clause->literals.end(), new_clause->literals.begin(), new_clause->literals.end(), std::inserter(testing_set1, testing_set1.begin()));
                            std::set_union(clause->literals_negated.begin(), clause->literals_negated.end(), new_clause->literals_negated.begin(), new_clause->literals_negated.end(), std::inserter(testing_set1, testing_set1.begin()));
                            if (testing_set1.size() == new_clause->literals.size() || testing_set2.size() == new_clause->literals_negated.size()) {
                                flag = false;
                                new_clause->~Clause();
                                break;
                            }
                        }

                        if (flag) {
                            curr_index++;
                            new_clause->index = curr_index;
                            resolvents.insert(new_clause);
                            testing_set.insert(new_clause);
                        }
                    }
                }

                for (auto clause : resolvents) {
                    set_of_support.insert(clause);
                    if (clause->literals.size() == 0 && clause->literals_negated.size() == 0) {
                        // todo NIL
                        //resolution_print_results(false);
                        std::cout << "true" << std::endl;
                        return;
                    }
                }
            }
        }
        if (set_of_support_size == set_of_support.size()) {
            //resolution_print_results(false);
            std::cout << "false" << std::endl;
            return;
        }
    }
}

void cooking() {

}

int main(int argc, char* argv[]){
    if (argc != 3 && argc != 4) exit(1);
    read_arguments(argc, argv);
    if (mode == "resolution") resolution();
    else if (mode == "cooking") cooking();
}