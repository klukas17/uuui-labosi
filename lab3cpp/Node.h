//
// Created by mihael on 4/27/22.
//

#ifndef LAB3CPP_NODE_H
#define LAB3CPP_NODE_H

#include "map"
#include "string"
#include "vector"

class Node {
public:
    bool is_leaf;
    std::string result;
    std::string feature;
    std::string feature_selected;
    std::map<std::string, Node*> children;
    std::string default_value;

    Node();
    ~Node();

    void print(std::vector<Node*> s);
};


#endif //LAB3CPP_NODE_H
