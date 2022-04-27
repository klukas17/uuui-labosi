//
// Created by mihael on 4/27/22.
//

#include "ID3.h"
#include "Node.h"
#include "math.h"
#include "set"
#include "string.h"
#include "iostream"

ID3::ID3(int d) {
    depth = d;
    root = new Node();
}
ID3::~ID3() {}

double log(double x) {
    if (x > 0) return x * log2(x);
    else return 0;
}

void build_tree(Node* node, std::vector<Unit*> data, std::set<std::string> unresolved_features, int depth) {

    if (depth == 0) {
        node->is_leaf = true;
        std::map<std::string, int> m;
        for (auto example : data)
            m[example->decision]++;
        std::string chosen_result;
        int count = -1;
        for (const auto &pair : m) {
            if (pair.second > count || (pair.second == count && pair.first < chosen_result)) {
                chosen_result = pair.first;
                count = pair.second;
            }
        }
        node->result = chosen_result;
        return;
    }

    std::map<std::string, int> partitions;
    for (auto example : data)
        partitions[example->decision]++;
    double entropy = 0;
    double size = data.size();
    for (const auto &p : partitions)
        entropy -= log(p.second/size);
    int default_count = -1;
    std::string default_choice;
    for (const auto &p : partitions) {
        if (p.second > default_count || (p.second == default_count && p.first < default_choice)) {
            default_choice = p.first;
            default_count = p.second;
        }
    }
    node->default_value = default_choice;

    if (entropy == 0) {
        node->is_leaf = true;
        for (const auto &pair : partitions)
            node->result = pair.first;
        return;
    }

    double information_gain = -1;
    std::string chosen_feature;
    std::map<std::string, std::vector<Unit*>> chosen_partitions;

    for (auto feature : unresolved_features) {
        std::map<std::string, std::vector<Unit*>> partitions;
        for (auto example : data)
            partitions[example->features[feature]].push_back(example);
        double e = entropy;

        for (const auto &pair : partitions) {
            std::map<std::string, int> branch_partitions;
            for (auto example : pair.second)
                branch_partitions[example->decision]++;
            double branch_entropy = 0;
            double branch_size = pair.second.size();
            for (const auto &p : branch_partitions)
                branch_entropy -= log(p.second/branch_size);
            e -= branch_entropy * branch_size/size;
        }

        std::cout << "IG(" << feature << ")=" << e << " ";

        if (e > information_gain) {
            information_gain = e;
            chosen_feature = feature;
            chosen_partitions = partitions;
        }
    }
    std::cout << "chosen:" << chosen_feature << std::endl;

    node->feature = chosen_feature;
    unresolved_features.erase(chosen_feature);

    for (const auto &pair : chosen_partitions) {
        Node* n = new Node();
        n->feature_selected = pair.first;
        build_tree(n, pair.second, unresolved_features, depth - 1);
        node->children[pair.first] = n;
    }
}

void ID3::fit(std::vector<Unit*> train_dataset) {
    std::set<std::string> unresolved_features;
    for (auto feature : features)
        if (feature != goal)
            unresolved_features.insert(feature);
    build_tree(root, train_dataset, unresolved_features, depth);
}

std::vector<std::string> ID3::predict(std::vector<Unit*> test_dataset) {
    std::vector<std::string> predictions;
    for (auto unit : test_dataset) {
        Node* n = root;
        bool flag = true;
        while (!n->is_leaf) {
            if (n->children[unit->features[n->feature]])
                n = n->children[unit->features[n->feature]];
            else {
                flag = false;
                predictions.push_back(n->default_value);
                break;
            }
        }
        if (flag)
            predictions.push_back(n->result);
    }
    return predictions;
}