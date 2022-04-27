//
// Created by mihael on 4/27/22.
//

#ifndef LAB3CPP_UNIT_H
#define LAB3CPP_UNIT_H

#include "map"
#include "string"

class Unit {
public:
    std::map<std::string, std::string> features;
    std::string decision;

    Unit();
    ~Unit();
};


#endif //LAB3CPP_UNIT_H
