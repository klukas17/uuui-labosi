//
// Created by mihael on 3/12/22.
//

#ifndef LAB1CPP_BFS_NODE_H
#define LAB1CPP_BFS_NODE_H

#include <iostream>

class Node {
public:
    std::string state;
    Node* prev;
    double cost;
    double f; // only for A*

    Node(std::string s, Node* p, double c);
    ~Node();

    int path_length();
    double total_cost();
    void print_path();
};


#endif //LAB1CPP_BFS_NODE_H
