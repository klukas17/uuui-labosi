//
// Created by mihael on 3/21/22.
//

#ifndef LAB2CPP_CLAUSE_H
#define LAB2CPP_CLAUSE_H

#include <set>
#include <string>

class Clause {
public:
    std::set<std::string> literals;
    std::set<std::string> literals_negated;
    std::set<Clause*> parent_clausules;
    int index;

    Clause();
    ~Clause();
};


#endif //LAB2CPP_CLAUSE_H