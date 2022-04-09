//
// Created by mihael on 3/21/22.
//

#include <iostream>

#include "Clause.h"

Clause::Clause() {
    printed = false;
    negated_result = false;
}
Clause::~Clause() {}

void Clause::print_clause(bool print_parents) {

    unsigned long int literals_count = 0;
    unsigned long int literals_negated_count = 0;

    for (auto c : literals) {
        std::cout << c;
        literals_count++;
        if (literals_count < literals.size())
            std::cout << " v ";
    }

    if (literals.size() > 0 && literals_negated.size() > 0)
        std::cout << " v ";

    for (auto c : literals_negated) {
        std::cout << "~" << c;
        literals_negated_count++;
        if (literals_negated_count < literals_negated.size())
            std::cout << " v ";
    }

    if (literals.size() == 0 && literals_negated.size() == 0)
        std::cout << "NIL";

    if (print_parents && parent_clauses.size() > 0) {
        unsigned long int done = 0;
        std::cout << " (";
        for (auto p : parent_clauses) {
            done++;
            p->print_clause(false);
            if (done < parent_clauses.size())
                std::cout << " && ";
        }
        std::cout << ")";
    }

    if (print_parents)
        std::cout << std::endl;
}