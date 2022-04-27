#include "iostream"
#include "iomanip"
#include "vector"
#include "fstream"
#include "climits"
#include "set"
#include "Unit.h"
#include "ID3.h"

std::vector<Unit*> train_dataset, test_dataset;
std::string train_dataset_path, test_dataset_path;
int depth;
std::set<std::string> all_results;

void read_arguments(int argc, char* argv[]) {
    train_dataset_path = argv[1];
    test_dataset_path = argv[2];
    if (argc == 3) {
        depth = INT_MAX;
    }
    else {
        depth = std::stoi(argv[3]);
    }
}

void read_training_data(ID3* model) {
    std::ifstream input(train_dataset_path);
    std::string line;
    bool attributes_read = false;

    while (getline(input, line)) {

        if (!attributes_read) {
            attributes_read = true;
            std::istringstream iss(line);
            std::string s;

            while (getline(iss, s, ','))
                model->features.push_back(s);
            model->goal = s;
        }

        else {
            Unit* u = new Unit();
            train_dataset.push_back(u);

            std::istringstream iss(line);
            std::string s;

            for (int i = 0; getline(iss, s, ','); i++)
                u->features[model->features[i]] = s;
            u->decision = s;
            all_results.insert(s);
        }
    }
}

void read_testing_data(ID3* model) {
    std::ifstream input(test_dataset_path);
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
            Unit* u = new Unit();
            test_dataset.push_back(u);

            std::istringstream iss(line);
            std::string s;

            for (int i = 0; getline(iss, s, ','); i++)
                u->features[model->features[i]] = s;
            u->decision = s;
        }
    }
}

int main(int argc, char* argv[]){
    read_arguments(argc, argv);
    ID3* model = new ID3(depth);
    read_training_data(model);
    read_testing_data(model);
    model->fit(train_dataset);

    std::cout << std::fixed;
    std::cout << std::setprecision(5);

    std::cout << "[BRANCHES]:" << std::endl;
    model->root->print({});

    std::cout << "[PREDICTIONS]:";
    std::vector<std::string> predictions = model->predict(test_dataset);
    for (auto prediction : predictions)
        std::cout << " " << prediction;
    std::cout << std::endl;

    std::cout << "[ACCURACY]: ";
    int accurate = 0;
    for (long unsigned int i = 0; i < predictions.size(); i++)
        if (predictions[i] == test_dataset[i]->decision)
            accurate++;
    std::cout << accurate / (double) predictions.size() << std::endl;

    std::cout << "[CONFUSION_MATRIX]:" << std::endl;
    std::map<std::string, std::map<std::string, int>> confusion_matrix;
    std::vector<std::string> v(all_results.begin(), all_results.end());
    for (auto x : v)
        for (auto y : v)
            confusion_matrix[x][y] = 0;
    for (long unsigned int i = 0; i < predictions.size(); i++) {
        confusion_matrix[test_dataset[i]->decision][predictions[i]]++;
    }
    for (const auto &p1 : confusion_matrix) {
        for (const auto &p2 : p1.second) {
            std::cout << p2.second << " ";
        }
        std::cout << std::endl;
    }
}
