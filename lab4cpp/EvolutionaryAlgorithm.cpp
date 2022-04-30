//
// Created by mihael on 4/29/22.
//

#include "EvolutionaryAlgorithm.h"
#include "iostream"
#include "chrono"
#include "random"

EvolutionaryAlgorithm::EvolutionaryAlgorithm(int p_s, int e_c, int i_c, double p_, double K_, std::vector <std::vector<double>> train, std::vector <std::vector<double>> test, std::vector<int> h_l_s, int i_d) {
    population_size = p_s;
    elitism_count = e_c;
    iteration_count = i_c;
    p = p_;
    K = K_;
    train_data = train;
    test_data = test;
    hidden_layers_sizes = h_l_s;
    input_dimension = i_d;
}

EvolutionaryAlgorithm::~EvolutionaryAlgorithm() {}

void EvolutionaryAlgorithm::initialize_population() {
    population = std::vector<Unit*>();
    for (int i = 0; i < population_size; i++) {
        population.push_back(new Unit(hidden_layers_sizes, input_dimension, i + 1));
    }
}

Unit *EvolutionaryAlgorithm::create_new_unit(int ind) {
    double f = 0;
    for (auto x : population)
        f += x->fitness;

    std::vector<Unit*> crossover_parents;
    double r, acc;
    int index;

    for (int i = 0; i < 2; i++) {
        r = ((double) rand() / RAND_MAX) * f;
        acc = 0;
        index = -1;
        while (acc < r) {
            index++;
            acc += population[index]->fitness;
        }
        crossover_parents.push_back(population[index]);
    }

    Unit* u = new Unit(ind);
    std::vector<Eigen::MatrixXd> matrices;

    for (unsigned long int j = 0; j < crossover_parents[0]->genotype->matrices.size(); j++)
        matrices.push_back((crossover_parents[0]->genotype->matrices[j] + crossover_parents[1]->genotype->matrices[j]) / 2);

    u->genotype = new NeuralNet(matrices);

    if ((double) rand() / RAND_MAX < p) {
        std::default_random_engine generator;
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
        std::normal_distribution<double> distribution(0, K);

        for (unsigned long k = 0; k < u->genotype->matrices.size(); k++)
            for (int i = 0; i < u->genotype->matrices[k].rows(); i++)
                for (int j = 0; j < u->genotype->matrices[k].cols() - 1; j++)
                    u->genotype->matrices[k](i,j) += distribution(generator);
    }

    return u;
}

void EvolutionaryAlgorithm::train() {
    for (int i = 1; i <= iteration_count; i++) {

        for (auto unit : population) {
            double error = 0;
            for (auto test : train_data) {
                Eigen::MatrixXd v(1, test.size());
                for (unsigned long int i = 0; i < test.size() - 1; i++)
                    v(0, i) = test[i];
                v(0, test.size() - 1) = 1;
                double res = unit->genotype->forward_pass(v), t = test[test.size() - 1];
                double err = (res - t);
                error += err * err;
            }
            unit->square_error = error / train_data.size();
            unit->fitness = 1 / unit->square_error;
        }

        std::sort(population.begin(), population.end(), [](Unit* u1, Unit* u2) {
            if (u1->square_error != u2->square_error) return (u1->square_error < u2->square_error);
            else return (u1->index < u2->index);
        });

        if (i % 100 == 0) {
            std::cout << "[Train error @" << i << "]: " << population[0]->square_error << std::endl;
        }

        if (i != iteration_count) {
            std::vector<Unit*> new_population;

            for (int j = 0; j < elitism_count; j++)
                new_population.push_back((population[j]));

            for (int j = elitism_count; j < population_size; j++) {
                Unit* u = create_new_unit(j + 1);
                new_population.push_back(u);
            }

            for (int j = elitism_count; j < population_size; j++)
                delete population[j];

            population = new_population;
        }
    }
}

void EvolutionaryAlgorithm::test() {
    for (auto unit : population) {
        double error = 0;
        for (auto test : test_data) {
            Eigen::MatrixXd v(1, test.size());
            for (unsigned long int i = 0; i < test.size() - 1; i++)
                v(0, i) = test[i];
            v(0, test.size() - 1) = 1;
            double res = unit->genotype->forward_pass(v), t = test[test.size() - 1];
            double err = (res - t);
            error += err * err;
        }
        unit->square_error = error / train_data.size();
        unit->fitness = 1 / unit->square_error;
    }

    std::sort(population.begin(), population.end(), [](Unit* u1, Unit* u2) {
        if (u1->square_error != u2->square_error) return (u1->square_error > u2->square_error);
        else return (u1->index > u2->index);
    });

    std::cout << "[Test error]: " << population[0]->square_error << std::endl;
}
