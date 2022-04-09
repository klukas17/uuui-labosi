#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stack>

#include "Clause.h"
#include "Command.h"

std::string mode;
std::string clauses_path;
std::string commands_path;
std::set<Clause*> clauses;
std::set<Clause*> set_of_support;

std::string testing_clause;
std::stack<Clause*> final_clauses;

std::vector<Command*> commands;

void read_arguments(int argc, char* argv[]) {
    mode = argv[1];
    clauses_path = argv[2];
    if (argc == 4) commands_path = argv[3];
}

void read_clauses() {
    std::ifstream input(clauses_path);
    std::string line;
    Clause* last_clause = nullptr;
    while (getline(input, line)) {

        if (line[0] == '#') continue;

        std::transform(line.begin(), line.end(), line.begin(), [](char c){return std::tolower(c);});

        testing_clause = line;

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
            if (last_clause)
                clauses.insert(last_clause);
            last_clause = new_clause;
        }
    }

    if (mode == "resolution") {
        std::set<std::string> literals = last_clause->literals;
        std::set<std::string> literals_negated = last_clause->literals_negated;

        for (std::string literal : literals) {
            Clause* c = new Clause();
            c->literals_negated.insert(literal);
            c->negated_result = true;
            set_of_support.insert(c);
        }
        for (std::string literal_negated : literals_negated) {
            Clause* c = new Clause();
            c->literals.insert(literal_negated);
            c->negated_result = true;
            set_of_support.insert(c);
        }

        delete last_clause;
    }

    else if (mode == "cooking") {
        clauses.insert(last_clause);
    }
}

void read_commands() {
    std::ifstream input(commands_path);
    std::string line;

    while (getline(input, line)) {
        if (line[0] == '#') continue;
        std::transform(line.begin(), line.end(), line.begin(), [](char c){return std::tolower(c);});

        std::istringstream iss(line);
        std::string s;
        bool skip = false;
        Command* new_command = new Command();
        new_command->full_text = line;
        while (getline(iss, s, ' ')) {
            if (skip) {
                skip = false;
                if (s == "+" || s == "-" || s == "?") new_command->command = s;
                continue;
            }
            skip = true;
            if (s[0] != '~')
                new_command->literals.insert(s);
            else
                new_command->literals_negated.insert(s.substr(1, s.size()));
        }

        commands.push_back(new_command);
    }
}

void resolution_print_results_true() {

    std::vector<Clause*> derived_clauses;
    std::vector<Clause*> negated_result;

    while (final_clauses.size() > 0) {
        auto x = final_clauses.top();
        final_clauses.pop();
        if (x->parent_clauses.size() == 0) {
            if (!x->printed) {
                if (!x->negated_result) {
                    x->print_clause(true);
                    x->printed = true;
                }
                else {
                    negated_result.push_back(x);
                }
            }
        }
        else
            derived_clauses.push_back(x);
    }

    for (auto x : negated_result) {
        if (!x->printed) {
            x->print_clause(true);
            x->printed = true;
        }
    }

    std::cout << "===============" << std::endl;

    for (auto x : derived_clauses)
        if (!x->printed) {
            x->print_clause(true);
            x->printed = true;
        }
    std::cout << "===============" << std::endl;
}

void resolution() {

    std::set<Clause*> checked_clauses;
    for (Clause* c : clauses) checked_clauses.insert(c);

    while (true) {
        std::set<Clause*> new_clauses;
        std::set<Clause*> SOS_U_clauses;
        std::set_union(set_of_support.begin(), set_of_support.end(), checked_clauses.begin(), checked_clauses.end(), std::inserter(SOS_U_clauses, SOS_U_clauses.begin()));

        for (Clause* c1 : set_of_support) {
            for (Clause* c2 : SOS_U_clauses) {
                if (c1 == c2) continue;

                std::set<std::string> s1, s2;
                std::set_intersection(c1->literals.begin(), c1->literals.end(), c2->literals_negated.begin(), c2->literals_negated.end(), std::inserter(s1, s1.begin()));
                std::set_intersection(c1->literals_negated.begin(), c1->literals_negated.end(), c2->literals.begin(), c2->literals.end(), std::inserter(s2, s2.begin()));

                if (s1.size() + s2.size() != 1) continue;

                Clause* new_clause = new Clause();

                if (s1.size() == 1) {
                    std::string intersect = *s1.begin();
                    for (std::string s : c1->literals) if (s != intersect) new_clause->literals.insert(s);
                    for (std::string s : c1->literals_negated) new_clause->literals_negated.insert(s);
                    for (std::string s : c2->literals) new_clause->literals.insert(s);
                    for (std::string s : c2->literals_negated) if (s != intersect) new_clause->literals_negated.insert(s);
                }

                if (s2.size() == 1) {
                    std::string intersect = *s2.begin();
                    for (std::string s : c1->literals) new_clause->literals.insert(s);
                    for (std::string s : c1->literals_negated) if (s != intersect) new_clause->literals_negated.insert(s);
                    for (std::string s : c2->literals) if (s != intersect) new_clause->literals.insert(s);
                    for (std::string s : c2->literals_negated) new_clause->literals_negated.insert(s);
                }

                new_clause->parent_clauses.push_back(c1);
                new_clause->parent_clauses.push_back(c2);

                if (new_clause->literals.size() == 0 && new_clause->literals_negated.size() == 0) {

                    std::stack<Clause*> s;
                    s.push(new_clause);
                    Clause* curr = nullptr;

                    while (s.size() > 0) {
                        curr = s.top();
                        s.pop();

                        final_clauses.push(curr);
                        if (curr->parent_clauses.size() > 0) {
                            for (Clause* p : curr->parent_clauses) {
                                s.push(p);
                            }
                        }
                    }

                    resolution_print_results_true();
                    std::cout << "[CONCLUSION]: " << testing_clause << " is true" << std::endl;
                    return;
                }

                std::set<std::string> tautology_check_set;
                std::set_intersection(new_clause->literals.begin(), new_clause->literals.end(), new_clause->literals_negated.begin(), new_clause->literals_negated.end(), std::inserter(tautology_check_set, tautology_check_set.begin()));

                if (tautology_check_set.size() > 0)
                    delete new_clause;

                else {

                    bool flag = true;
                    std::vector<Clause*> clauses_to_remove_from_new_clauses;

                    for (Clause* c : new_clauses) {

                        bool not_subset = false;
                        for (std::string literal : new_clause->literals) {
                            if (c->literals.find(literal) == c->literals.end()) {
                                not_subset = true;
                                break;
                            }
                        }
                        if (!not_subset) for (std::string literal_negated : new_clause->literals_negated) {
                            if (c->literals_negated.find(literal_negated) == c->literals_negated.end()) {
                                not_subset = true;
                                break;
                            }
                        }
                        if (!not_subset) {
                            clauses_to_remove_from_new_clauses.push_back(c);
                            continue;
                        }

                        not_subset = false;
                        for (std::string literal : c->literals) {
                            if (new_clause->literals.find(literal) == new_clause->literals.end()) {
                                not_subset = true;
                                break;
                            }
                        }
                        if (!not_subset) for (std::string literal_negated : c->literals_negated) {
                            if (new_clause->literals_negated.find(literal_negated) == new_clause->literals_negated.end()) {
                                not_subset = true;
                                break;
                            }
                        }
                        if (!not_subset) {
                            flag = false;
                            delete new_clause;
                            break;
                        }

                    }

                    if (flag) {
                        for (Clause* clause : clauses_to_remove_from_new_clauses) new_clauses.erase(clause);
                        new_clauses.insert(new_clause);
                    }
                }
            }
        }

        for (Clause* clause : set_of_support)
            checked_clauses.insert(clause);

        std::vector<Clause*> clauses_to_remove_from_new_clauses;
        for (Clause* new_clause : new_clauses) {
            for (Clause* c : checked_clauses) {
                bool not_subset = false;
                for (std::string literal : c->literals) {
                    if (new_clause->literals.find(literal) == new_clause->literals.end()) {
                        not_subset = true;
                        break;
                    }
                }
                if (!not_subset) for (std::string literal_negated : c->literals_negated) {
                    if (new_clause->literals_negated.find(literal_negated) == new_clause->literals_negated.end()) {
                        not_subset = true;
                        break;
                    }
                }
                if (!not_subset) {
                    clauses_to_remove_from_new_clauses.push_back(new_clause);
                }
            }
        }
        for (Clause* clause : clauses_to_remove_from_new_clauses)
            new_clauses.erase(clause);

        if (new_clauses.size() == 0) {
            std::cout << "[CONCLUSION]: " << testing_clause << " is unknown" << std::endl;
            return;
        }

        set_of_support = new_clauses;

        new_clauses = std::set<Clause*>();
    }
}

void cooking() {

    std::cout << "Constructed with knowledge:" << std::endl;
    for (Clause* c : clauses) c->print_clause(true);

    for (Command* command : commands) {

        std::cout << std::endl << "User's: command: " << command->full_text << std::endl;

        if (command->command == "+") {
            bool found = false;
            for (Clause* clause : clauses) {
                if (clause->literals == command->literals && clause->literals_negated == command->literals_negated) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                Clause* c = new Clause();
                c->literals = command->literals;
                c->literals_negated = command->literals_negated;
                clauses.insert(c);
                std::cout << "Added " << command->full_text.substr(0, command->full_text.size() - 2) << std::endl;
            }
            else std::cout << command->full_text.substr(0, command->full_text.size() - 2) << " is already true" << std::endl;
        }
        else if (command->command == "-") {
            bool found = false;
            Clause* c = nullptr;
            for (Clause* clause : clauses) {
                if (clause->literals == command->literals && clause->literals_negated == command->literals_negated) {
                    found = true;
                    c = clause;
                    break;
                }
            }
            if (found) {
                clauses.erase(c);
                std::cout << "Removed " << command->full_text.substr(0, command->full_text.size() - 2) << std::endl;
            }
            else std::cout << command->full_text.substr(0, command->full_text.size() - 2) << " was not true" << std::endl;
        }
        else if (command->command == "?") {
            std::set<std::string> literals = command->literals;
            std::set<std::string> literals_negated = command->literals_negated;

            set_of_support = std::set<Clause*>();

            for (std::string literal : literals) {
                Clause* c = new Clause();
                c->literals_negated.insert(literal);
                c->negated_result = true;
                set_of_support.insert(c);
            }
            for (std::string literal_negated : literals_negated) {
                Clause* c = new Clause();
                c->literals.insert(literal_negated);
                c->negated_result = true;
                set_of_support.insert(c);
            }

            testing_clause = command->full_text.substr(0, command->full_text.size() - 2);

            resolution();

            for (Clause* clause : clauses)
                clause->printed = false;
        }
    }
}

int main(int argc, char* argv[]){
    if (argc != 3 && argc != 4) exit(1);
    read_arguments(argc, argv);
    if (mode == "resolution") {
        read_clauses();
        resolution();
    }
    else if (mode == "cooking") {
        read_clauses();
        read_commands();
        cooking();
    }
}