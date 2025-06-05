#include <bits/stdc++.h>
using namespace std;

class Attribute {
    public:
    int id;
    vector<string> val;
};

class ExampleSet {
    public:
    vector<string*> classes[4];
};

class Node {
    public:
    int id = -1;
    vector<Node*> child;
    int clas = -1;
};

Attribute attributes[6];
map<string, int> classMap;
int SIZE = 6;
int availability = SIZE;
int isAvailable[6];

// Calculate Gini Impurity
double gini_impurity(ExampleSet ex, Attribute a) {
    int children[a.val.size()][4] = {0};
    int totalEx = 0;
    int exPerChild[a.val.size()] = {0};

    for (int k = 0; k < 4; k++) { // for classes
        for (int i = 0; i < ex.classes[k].size(); i++) { // for examples in each class
            for (int j = 0; j < a.val.size(); j++) { // for each value
                if (ex.classes[k][i][a.id] == a.val[j]) {
                    children[j][k]++;
                    exPerChild[j]++; 
                    break;
                }
            }
            totalEx++;
        }
    }

    // Calculate Gini impurity for children
    double gini = 0.0;
    for (int j = 0; j < a.val.size(); j++) {
        double sum = 0.0;
        for (int k = 0; k < 4; k++) {
            double p = (double)children[j][k] / (double)exPerChild[j];
            sum += p * p;
        }
        gini += ((double)exPerChild[j] / (double)totalEx) * (1.0 - sum);
    }
    return gini;
}

// Calculate Information Gain
double information_gain(ExampleSet ex, Attribute a) {
    int children[a.val.size()][4] = {0};
    int totalEx = 0;
    int exPerChild[a.val.size()] = {0};

    for (int k = 0; k < 4; k++) { // for classes
        for (int i = 0; i < ex.classes[k].size(); i++) { // for examples in each class
            for (int j = 0; j < a.val.size(); j++) { // for each value
                if (ex.classes[k][i][a.id] == a.val[j]) {
                    children[j][k]++;
                    exPerChild[j]++;
                    break;
                }
            }
            totalEx++;
        }
    }

    // Parent entropy
    double parentEntropy = 0;
    for (int k = 0; k < 4; k++) {
        double p = (double)ex.classes[k].size() / (double)totalEx;
        if (p > 0)
            parentEntropy += p * log2(p) * -1.0;
    }

    // Children entropy
    double remainder = 0;
    for (int j = 0; j < a.val.size(); j++) {
        double sum = 0;
        for (int k = 0; k < 4; k++) {
            double p = (double)children[j][k] / (double)exPerChild[j];
            if (p > 0)
                sum += p * log2(p) * -1.0;
        }
        remainder += ((double)exPerChild[j] / (double)totalEx) * sum;
    }

    return parentEntropy - remainder;
}

// Select attribute based on criteria and strategy
Attribute* importance(ExampleSet ex, string criteria, bool randomTop3) {
    vector<pair<double, int>> gains;

    for (int i = 0; i < SIZE; i++) {
        if (isAvailable[i]) {
            double g = (criteria == "information_gain") ? information_gain(ex, attributes[i]) : -gini_impurity(ex, attributes[i]);
            gains.push_back({g, i});
        }
    }

    // Sort attributes by gain
    sort(gains.begin(), gains.end(), greater<pair<double, int>>());

    // Select based on strategy
    int idx = 0;
    if (randomTop3 && gains.size() >= 3) {
        idx = rand() % 3;
    }
    return &attributes[gains[idx].second];
}

// Create decision tree
void create_decision_tree(ExampleSet ex, ExampleSet parent, Node* node, string criteria, bool randomTop3) {
    int isOne = -1, size = 0;
    for (int k = 0; k < 4; k++) {
        if (ex.classes[k].size() != 0 && isOne == -1) {
            isOne = k;
        } else if (ex.classes[k].size() != 0 && isOne != -1) {
            isOne = -1;
            size += ex.classes[k].size();
            break;
        }
        size += ex.classes[k].size();
    }

    if (size == 0) { // Empty examples
        int maxSize = -1;
        int idx;
        for (int k = 0; k < 4; k++) {
            if (parent.classes[k].size() > maxSize) {
                maxSize = parent.classes[k].size();
                idx = k;
            }
        }
        node->clas = idx;
        return;
    } else if (isOne != -1) { // All examples belong to one class
        node->clas = isOne;
        return;
    } else if (availability == 0) { // No attributes left
        int maxSize = -1;
        int idx;
        for (int k = 0; k < 4; k++) {
            if (ex.classes[k].size() > maxSize) {
                maxSize = ex.classes[k].size();
                idx = k;
            }
        }
        node->clas = idx;
        return;
    } else {
        Attribute* a = importance(ex, criteria, randomTop3);
        isAvailable[a->id] = 0;
        availability--;
        node->id = a->id;

        for (int j = 0; j < a->val.size(); j++) {
            ExampleSet b;
            Node* newNode = new Node();
            for (int k = 0; k < 4; k++) { // Split examples based on attribute
                for (int i = 0; i < ex.classes[k].size(); i++) {
                    if (ex.classes[k][i][a->id] == a->val[j]) {
                        b.classes[k].push_back(ex.classes[k][i]);
                    }
                }
            }
            create_decision_tree(b, ex, newNode, criteria, randomTop3);
            node->child.push_back(newNode);
        }
        isAvailable[a->id] = 1;
        availability++;
    }
}

Node* traverse_tree(Node* node, string example[]) {
    if (node->child.size() == 0) {
        return node;
    } else {
        for (int i = 0; i < attributes[node->id].val.size(); i++) {
            //Check if the current value matches the example's value for this attribute
            if (attributes[node->id].val[i] == example[node->id]) {
                return traverse_tree(node->child[i], example);
            }
        }
        return node;
    }
}

int main() {
    ifstream input;
    input.open("car.data");
    string line;
    int no_examples = 0;
    int iteration = 20;
    classMap["unacc"] = 0;
    classMap["acc"] = 1;
    classMap["good"] = 2;
    classMap["vgood"] = 3;
    string inputData[2000][7];

    while (getline(input, line)) {
        stringstream iss(line);
        string token;
        int j = 0;
        while (getline(iss, token, ',')) {
            inputData[no_examples][j++] = token;
        }
        no_examples++;
    }
    input.close();

    attributes[0].val = {"vhigh", "high", "med", "low"};
    attributes[1].val = {"vhigh", "high", "med", "low"};
    attributes[2].val = {"2", "3", "4", "5more"};
    attributes[3].val = {"2", "4", "more"};
    attributes[4].val = {"small", "med", "big"};
    attributes[5].val = {"low", "med", "high"};

    srand(time(0));
    int selected_no = no_examples * 0.8;

    vector<string> criteria = {"information_gain", "gini_impurity"};
    vector<bool> strategies = {false, true}; // Always select the best, select from top 3

    for (auto crit : criteria) {
        for (auto strategy : strategies) {
            double total = 0.0;
            double accuracy[iteration];
            for (int it = 0; it < iteration; it++) {
                vector<string*> examples;
                for (int p = 0; p < no_examples; p++)
                    examples.push_back(inputData[p]);

                for (int k = 0; k < SIZE; k++) {
                    attributes[k].id = k;
                    isAvailable[k] = 1;
                }

                ExampleSet exampleSet;
                for (int i = 0; i < selected_no; i++) {
                    int r = rand() % examples.size();
                    int idx = classMap[examples[r][SIZE]];
                    exampleSet.classes[idx].push_back(examples[r]);
                    examples.erase(examples.begin() + r);
                }

                ExampleSet parent;
                Node root;
                create_decision_tree(exampleSet, parent, &root, crit, strategy);

                int correct = 0;
                for (int i = 0; i < examples.size(); i++) {
                    Node* node = traverse_tree(&root, examples[i]);
                    if (node->clas == classMap[examples[i][SIZE]])
                        correct++;
                }
                accuracy[it] = (double)correct / (double)examples.size();
                total += accuracy[it];
            }
            double average = total / (double)iteration;

            double squareSum = 0.0;
            for (int i = 0; i < iteration; i++) {
                double diff = average - accuracy[i];
                squareSum += diff * diff;
            }
            double dev = sqrt(squareSum / (double)iteration);
            average *= 100;
            dev *= 100;

            cout << "Criteria: " << crit << ", Strategy: " << (strategy ? "Top-3 Random" : "Best") << endl;
            cout << "Average Accuracy: " << average << "%" << endl;
            cout << "Standard Deviation: " << dev << "%" << endl;
            cout << "---------------------------------" << endl;
            cout << endl;
        }
    }
    return 0;
}
