//
// Created by mihael on 4/29/22.
//

#ifndef LAB4CPP_UNIT_H
#define LAB4CPP_UNIT_H

#include "NeuralNet.h"

class Unit {
public:
    NeuralNet* genotype;
    double fitness;
    double square_error;
    int index;

    Unit(std::vector<int> hidden_layer_sizes, int input_dimension, int ind);
    Unit(int ind);
    ~Unit();
};


#endif //LAB4CPP_UNIT_H
