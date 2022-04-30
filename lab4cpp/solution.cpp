#include <iostream>
#include "Eigen/Dense"

#include "string"
#include "vector"
#include "sstream"
#include "fstream"
#include "iomanip"

#include "EvolutionaryAlgorithm.h"

std::string train_path, test_path;
int population_size, elitism_count, iteration_count;
double p, K;
std::vector<int> hidden_layers_sizes;
std::vector<std::vector<double>> train_data;
std::vector<std::vector<double>> test_data;
int input_dimension;

void read_arguments(int argc, char* argv[]) {
    int index = 1;
    while (index < argc) {
        std::string argument = argv[index];
        if (argument == "--train") {
            train_path = argv[index + 1];
            index += 2;
            continue;
        } else if (argument == "--test") {
            test_path = argv[index + 1];
            index += 2;
            continue;
        } else if (argument == "--nn") {
            std::istringstream iss(argv[index + 1]);
            std::string s;
            while (getline(iss, s, 's'))
                hidden_layers_sizes.push_back(std::stoi(s));
        } else if (argument == "--popsize") {
            population_size = std::stoi(argv[index + 1]);
            index += 2;
            continue;
        } else if (argument == "--elitism") {
            elitism_count = std::stoi(argv[index + 1]);
            index += 2;
            continue;
        } else if (argument == "--p") {
            p = std::stod(argv[index + 1]);
            index += 2;
            continue;
        } else if (argument == "--K") {
            K = std::stod(argv[index + 1]);
            index += 2;
            continue;
        } else if (argument == "--iter") {
            iteration_count = std::stoi(argv[index + 1]);
            index += 2;
            continue;
        }
        index++;
    }
}

void read_train_data() {
    std::ifstream input(train_path);
    std::string line;
    bool attributes_read = false;
    input_dimension = 0;

    while (getline(input, line)) {

        if (!attributes_read) {
            attributes_read = true;
            std::istringstream iss(line);
            std::string s;
            while (getline(iss, s, ','))
                input_dimension++;
            input_dimension--;
        }

        else {
            std::istringstream iss(line);
            std::string s;
            std::vector<double> x;
            while (getline(iss, s, ','))
                x.push_back(std::stod(s));
            train_data.push_back(x);
        }
    }
}

void read_test_data() {
    std::ifstream input(train_path);
    std::string line;
    bool attributes_read = false;

    while (getline(input, line)) {

        if (!attributes_read) {
            attributes_read = true;
            std::istringstream iss(line);
            std::string s;
            while (getline(iss, s, ','));
        }

        else {
            std::istringstream iss(line);
            std::string s;
            std::vector<double> x;
            while (getline(iss, s, ','))
                x.push_back(std::stod(s));
            test_data.push_back(x);
        }
    }
}

int main(int argc, char* argv[]){
    read_arguments(argc, argv);
    read_train_data();
    read_test_data();

    srand(time(NULL));
    std::cout << std::fixed;
    std::cout << std::setprecision(6);

    EvolutionaryAlgorithm* ea = new EvolutionaryAlgorithm(population_size, elitism_count, iteration_count, p, K, train_data, test_data, hidden_layers_sizes, input_dimension);
    ea->initialize_population();
    ea->train();
    ea->test();
}
