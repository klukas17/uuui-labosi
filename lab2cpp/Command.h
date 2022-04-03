//
// Created by mihael on 4/3/22.
//

#ifndef LAB2CPP_COMMAND_H
#define LAB2CPP_COMMAND_H

#include "Clause.h"

class Command : public Clause {
public:
    std::string command;
    std::string full_text;

    Command();
    ~Command();
};


#endif //LAB2CPP_COMMAND_H
