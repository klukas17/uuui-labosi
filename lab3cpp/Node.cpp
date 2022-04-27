//
// Created by mihael on 4/27/22.
//

#include "Node.h"
#include "iostream"

Node::Node() {
    is_leaf = false;
}
Node::~Node() {}

void Node::print(std::vector<Node*> s) {
    if (is_leaf) {
        s.push_back(this);
        for (long unsigned int i = 0; i < s.size() - 1; i++) {
            Node* n = s[i];
            std::cout << i + 1 << ":" << n->feature << "=" << s[i+1]->feature_selected << " ";
        }
        std::cout << result << std::endl;
    }
    else {
        for (const auto &pair : children) {
            Node* child = pair.second;
            s.push_back(this);
            child->print(s);
            s.pop_back();
        }
    }
}