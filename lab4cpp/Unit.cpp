//
// Created by mihael on 4/29/22.
//

#include "Unit.h"
#include "random"
#include "iostream"
#include "chrono"

Unit::Unit(std::vector<int> hidden_layer_sizes, int input_dimension, int ind) {
    index = ind;
    fitness = 0;
    square_error = 0;

    int rows = input_dimension, cols = hidden_layer_sizes[0];
    std::vector<Eigen::MatrixXd> matrices;

    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::normal_distribution<double> distribution(0, 0.01);

    for (unsigned long int i = 1; i < hidden_layer_sizes.size() + 1; i++) {
        Eigen::MatrixXd m(rows + 1, cols + 1);

        for (int j = 0; j < rows; j++)
            m(j, cols) = 0;
        m(rows, cols) = 1;

        for (int j = 0; j <= rows; j++) {
            for (int k = 0; k < cols; k++) {
                m(j, k) = distribution(generator);
            }
        }

        matrices.push_back(m);

        if (i != hidden_layer_sizes.size()) {
            rows = cols;
            cols = hidden_layer_sizes[i];
        }
    }

    rows = cols;
    cols = 1;

    Eigen::MatrixXd m(rows + 1, cols + 1);

    for (int j = 0; j < rows; j++)
        m(j, cols) = 0;
    m(rows, cols) = 1;

    for (int j = 0; j <= rows; j++) {
        for (int k = 0; k < cols; k++) {
            m(j, k) = distribution(generator);
        }
    }

    matrices.push_back(m);

    genotype = new NeuralNet(matrices);
}

Unit::Unit(int ind) {
    index = ind;
    fitness = 0;
    square_error = 0;
}

Unit::~Unit() {}