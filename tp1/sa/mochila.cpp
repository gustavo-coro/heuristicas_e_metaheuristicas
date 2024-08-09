#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

using namespace std;

// Structure to represent an item with profit, weight, position, and cost-benefit ratio
struct Item {
    int profit, weight, position;
    float cost_benefit;
};

// Structure to represent a solution with selected items, total value, and total weight
struct Solution {
    string items;
    int value;
    int weight;
};

// Comparator to sort items based on cost-benefit ratio in descending order
bool compareByCostBenefit(Item i, Item j) {
    return (i.cost_benefit > j.cost_benefit);
}

// Comparator to sort items based on weight in ascending order
bool compareByWeight(Item i, Item j) {
    return (i.weight < j.weight);
}

// Function to create an initial solution with a single random item included
string createInitialSolution(int numItems) {
    string solution(numItems, '0');
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_int_distribution<int> dist{0, numItems - 1};

    int i = dist(mersenne_engine);
    solution[i] = '1';
    return solution;
}

// Function to check if a solution is valid (total weight does not exceed the maximum weight)
bool isSolutionValid(string solution, Item *items, int maxWeight) {
    int totalWeight = 0;

    for (int i = 0; i < solution.length(); i++) {
        if (totalWeight > maxWeight) {
            return false;
        }
        totalWeight += (solution[items[i].position] - '0') * items[i].weight;
    }
    return totalWeight <= maxWeight;
}

// Function to calculate the profit and weight of a solution
Solution calculateProfit(string solutionStr, Item *items, int maxWeight) {
    Solution solution;
    solution.items = solutionStr;
    solution.value = 0;
    solution.weight = 0;
    for (int i = 0; i < solutionStr.length(); i++) {
        solution.value += (solutionStr[items[i].position] - '0') * items[i].profit;
        solution.weight += (solutionStr[items[i].position] - '0') * items[i].weight;
    }
    return solution;
}

// Function to calculate the profit of a solution with penalty for exceeding max weight
Solution calculateProfitWithPenalty(string solutionStr, Item *items, int penalty, int maxWeight) {
    Solution solution;
    solution.items = solutionStr;
    solution.value = 0;
    solution.weight = 0;
    for (int i = 0; i < solutionStr.length(); i++) {
        solution.value += (solutionStr[items[i].position] - '0') * items[i].profit;
        solution.weight += (solutionStr[items[i].position] - '0') * items[i].weight;
    }
    solution.value -= penalty * max(0, solution.weight - maxWeight);
    return solution;
}

// Function to generate a neighbor solution by toggling the inclusion of an item
Solution generateNeighbor(Solution currentSolution, Item *items, int switchPosition, int maxWeight) {
    string solutionStr = currentSolution.items;
    solutionStr[switchPosition] = (solutionStr[switchPosition] == '0') ? '1' : '0';
    return calculateProfit(solutionStr, items, maxWeight);
}

// Function to generate a random neighbor solution
Solution generateRandomNeighbor(Solution currentSolution, Item *items, int numItems, int maxWeight) {
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_int_distribution<int> dist{0, numItems - 1};

    int switchPosition = dist(mersenne_engine);
    return generateNeighbor(currentSolution, items, switchPosition, maxWeight);
}

// Function to generate a random neighbor solution with penalty for exceeding max weight
Solution generateRandomNeighborWithPenalty(Solution currentSolution, Item *items, int penalty, int numItems, int maxWeight) {
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_int_distribution<int> dist{0, numItems - 1};

    int switchPosition = dist(mersenne_engine);
    return generateNeighbor(currentSolution, items, switchPosition, maxWeight);
}

// Function to calculate the acceptance probability for a worse solution in simulated annealing
bool calculateAcceptanceProbability(float temperature, int currentValue, int neighborValue) {
    const double EulerConstant = exp(1.0);
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_real_distribution<float> dist{0, 1};

    float r = dist(mersenne_engine);
    float exponent = (currentValue - neighborValue) / temperature;
    float probability = pow(EulerConstant, exponent);
    return r < probability;
}

// Main function
int main(int argc, char **argv) {
    srand(time(NULL));
    clock_t startTime;

    if (argc != 2) {
        printf("Error in parameter passing\n");
        return -1;
    }

    ifstream inputFile(argv[1]);
    int numItems, maxWeight;
    inputFile >> numItems >> maxWeight;

    int maxIterations = 400;
    float initialTemperature = 1000;
    float coolingRate = 0.99;
    int max_without_change = 350;

    Item *items = (Item *)malloc(sizeof(Item) * numItems);
    for (int i = 0; i < numItems; i++) {
        inputFile >> items[i].profit >> items[i].weight;
        items[i].cost_benefit = (float)items[i].weight / items[i].profit;
        items[i].position = i;
    }

    int penalty = 0;
    for (int i = 0; i < numItems; i++) {
        penalty += items[i].weight;
    }

    string initialSolutionStr(numItems, '0');
    Solution initialSolution;

    startTime = clock();

    initialSolutionStr = createInitialSolution(numItems);
    initialSolution = calculateProfit(initialSolutionStr, items, maxWeight);
    cout << "Initial Solution: ";
    cout << initialSolution.items << " - " << initialSolution.value << endl;

    initialSolution = calculateProfit(initialSolutionStr, items, maxWeight);

    Solution bestSolution = initialSolution;
    float temperature = initialTemperature;
    int iterations = 0;
    int iter_without_change = 0;

    while ((temperature > 1e-10) && (iter_without_change < max_without_change)) {
        while (iterations < maxIterations) {
            iterations++;
            Solution neighbor = generateRandomNeighbor(initialSolution, items, numItems, maxWeight);

            if (isSolutionValid(neighbor.items, items, maxWeight)) {
                if (neighbor.value > initialSolution.value) {
                    initialSolution = neighbor;
                    if (neighbor.value > bestSolution.value) {
                        bestSolution = neighbor;
                        iter_without_change = 0;
                    }
                } else if (calculateAcceptanceProbability(temperature, initialSolution.value, neighbor.value)) {
                    initialSolution = neighbor;
                }
            }
        }
        cout << "Current best value: " << bestSolution.value << endl;
        temperature *= coolingRate;
        iterations = 0;
        iter_without_change++;
    }

    startTime = clock() - startTime;
    printf("Execution time: %f seconds.\n", ((float)startTime) / CLOCKS_PER_SEC);

    free(items);
    inputFile.close();
    return 0;
}
