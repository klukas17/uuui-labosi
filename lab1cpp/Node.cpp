//
// Created by mihael on 3/12/22.
//

#include "Node.h"

Node::Node(std::string s, Node* p, double c) {
    state = s;
    prev = p;
    cost = c;
    f = 0;
}

Node::~Node() {}

int Node::path_length() {
    if (prev == nullptr) return 1;
    else return prev->path_length() + 1;
}

double Node::total_cost() {
    return cost;
}

void Node::print_path() {
    if (prev != nullptr) {
        prev->print_path();
        std::cout << "=>";
    }
    std::cout << " " << state << " ";
}