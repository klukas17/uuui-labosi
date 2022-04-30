//
// Created by mihael on 4/29/22.
//

#ifndef LAB4CPP_NEURALNET_H
#define LAB4CPP_NEURALNET_H

#include "vector"
#include "Eigen/Dense"

class NeuralNet {
public:
    std::vector<Eigen::MatrixXd> matrices;

    NeuralNet(std::vector<Eigen::MatrixXd> m);
    ~NeuralNet();

    double forward_pass(Eigen::MatrixXd v);
};


#endif //LAB4CPP_NEURALNET_H
