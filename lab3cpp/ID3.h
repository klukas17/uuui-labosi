//
// Created by mihael on 4/27/22.
//

#ifndef LAB3CPP_ID3_H
#define LAB3CPP_ID3_H

#include "vector"
#include "string"
#include "Unit.h"
#include "Node.h"

class ID3 {
public:
    int depth;
    std::vector<std::string> features;
    std::string goal;
    Node* root;

    ID3(int d);
    ~ID3();

    void fit(std::vector<Unit*> train_dataset);
    std::vector<std::string> predict(std::vector<Unit*> test_dataset);
};


#endif //LAB3CPP_ID3_H
