//
// Created by mihael on 4/29/22.
//

#ifndef LAB4CPP_EVOLUTIONARYALGORITHM_H
#define LAB4CPP_EVOLUTIONARYALGORITHM_H

#include "vector"
#include "Unit.h"

class EvolutionaryAlgorithm {
public:
    int population_size;
    int elitism_count;
    int iteration_count;
    double p;
    double K;
    std::vector<std::vector<double>> train_data;
    std::vector<std::vector<double>> test_data;
    std::vector<Unit*> population;
    std::vector<int> hidden_layers_sizes;
    int input_dimension;

    EvolutionaryAlgorithm(int p_s, int e_c, int i_c, double p_, double K_, std::vector<std::vector<double>> train, std::vector<std::vector<double>> test, std::vector<int> h_l_s, int i_d);
    ~EvolutionaryAlgorithm();

    void initialize_population();
    void train();
    void test();

    Unit* create_new_unit(int ind);
};


#endif //LAB4CPP_EVOLUTIONARYALGORITHM_H
