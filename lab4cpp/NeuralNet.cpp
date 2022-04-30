//
// Created by mihael on 4/29/22.
//

#include "NeuralNet.h"

NeuralNet::NeuralNet(std::vector<Eigen::MatrixXd> m) {
    matrices = m;
}

NeuralNet::~NeuralNet() {}

double sigm(double x) {
    return 1 / (1 + exp(-x));
}

double NeuralNet::forward_pass(Eigen::MatrixXd v) {
    for (unsigned long int i = 0; i < matrices.size(); i++) {
        v = v * matrices[i];
        if (i != matrices.size() - 1) {
            v = v.unaryExpr(&sigm);
            v(0, v.cols() - 1) = 1;
        }
    }
    return v(0, 0);
}