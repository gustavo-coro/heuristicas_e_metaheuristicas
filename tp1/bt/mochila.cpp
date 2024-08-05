#include <bits/stdc++.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

// Structure to represent an item
struct Item {
    int profit, weight, index;
    float cost_benefit_ratio;
};

// Structure to represent a solution
struct Solution {
    string items;
    int value;
    int weight;
};

// Function to compare items by cost-benefit ratio in descending order
bool compareByCostBenefit(Item i, Item j) {
    return (i.cost_benefit_ratio > j.cost_benefit_ratio);
}

// Function to compare items by weight in ascending order
bool compareByWeight(Item i, Item j) { return (i.weight < j.weight); }

// Function to create an initial solution with a single item randomly selected
string createInitialSolution(int num_items) {
    string s(num_items, '0');
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_int_distribution<int> dist{0, num_items - 1};
    int i = dist(mersenne_engine);
    s[i] = '1';
    return s;
}

// Function to check if the solution is valid based on the maximum weight
// constraint
bool isValidSolution(string solution, Item* items, int max_weight) {
    int total_weight = 0;
    for (int i = 0; i < solution.length(); i++) {
        if (total_weight > max_weight) {
            return false;
        }
        total_weight += (solution[items[i].index] - '0') * items[i].weight;
    }
    return total_weight <= max_weight;
}

// Function to create a greedy solution based on cost-benefit ratio
string createGreedySolutionCB(int num_items, Item* items, int max_weight) {
    string solution = createInitialSolution(num_items);
    sort(items, items + num_items, compareByCostBenefit);
    for (int i = 0; i < num_items; i++) {
        solution[items[i].index] = '1';
        if (!isValidSolution(solution, items, max_weight)) {
            solution[items[i].index] = '0';
        }
    }
    return solution;
}

// Function to create a greedy solution based on weight
string createGreedySolutionLW(int num_items, Item* items, int max_weight) {
    string solution = createInitialSolution(num_items);
    sort(items, items + num_items, compareByWeight);
    for (int i = 0; i < num_items; i++) {
        solution[items[i].index] = '1';
        if (!isValidSolution(solution, items, max_weight)) {
            solution[items[i].index] = '0';
        }
    }
    return solution;
}

// Function to calculate the profit of a solution with a penalty for overweight
// solutions
Solution calculateProfit(string sol, Item* items, int penalty, int max_weight) {
    Solution s;
    s.items = sol;
    s.value = 0;
    s.weight = 0;
    for (int i = 0; i < sol.length(); i++) {
        s.value += (sol[items[i].index] - '0') * items[i].profit;
        s.weight += (sol[items[i].index] - '0') * items[i].weight;
    }
    s.value -= penalty * max(0, s.weight - max_weight);
    return s;
}

// Function to generate a neighboring solution by flipping the bit at the given
// index
Solution generateNeighbor(Solution sol, Item* items, int index, int penalty,
                          int max_weight) {
    string s = sol.items;
    s[index] = (s[index] == '0') ? '1' : '0';
    return calculateProfit(s, items, penalty, max_weight);
}

// Function to select a new solution from the neighborhood, considering tabu
// list
Solution selectNewSolution(Solution current, Solution best, vector<int>& tabu,
                           int tabu_size, int penalty, Item* items,
                           int num_items, int max_weight) {
    vector<Solution> neighborhood(num_items);
    for (int i = 0; i < num_items; i++) {
        neighborhood[i] =
            generateNeighbor(current, items, i, penalty, max_weight);
    }
    Solution new_solution;
    new_solution.value = std::numeric_limits<int>::min();
    int index, new_index;

    // Reset the oldest tabu entry if the list is full
    if (tabu_size == num_items) {
        int min_index = 0;
        for (int i = 0; i < num_items; i++) {
            if (tabu[min_index] == 0) break;
            if (tabu[i] < tabu[min_index]) {
                min_index = i;
            }
        }
        tabu[min_index] = 0;
    }

    // Select the best solution from the neighborhood
    for (index = 0; index < num_items; index++) {
        if ((tabu[index] == 0) || (neighborhood[index].value > best.value)) {
            if (neighborhood[index].value > new_solution.value) {
                new_solution = neighborhood[index];
                new_index = index;
            }
        }
    }

    // Update the tabu list
    for (index = 0; index < num_items; index++) {
        if (tabu[index] != 0) {
            tabu[index]--;
        }
    }
    tabu[new_index] = tabu_size;
    return new_solution;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    clock_t t;

    if (argc != 3) {
        printf("Error in parameter passing\n");
        return -1;
    }

    ifstream inputFile(argv[1]);
    int num_items, max_weight;
    inputFile >> num_items >> max_weight;

    Item* items = (Item*)malloc(sizeof(Item) * num_items);
    for (int i = 0; i < num_items; i++) {
        inputFile >> items[i].profit >> items[i].weight;
        items[i].cost_benefit_ratio =
            (float)items[i].profit / (float)items[i].weight;
        items[i].index = i;
    }

    int penalty = 0;
    for (int i = 0; i < num_items; i++) {
        penalty += items[i].weight;
    }

    string initial_solution;
    Solution sol;

    t = clock();

    if (*argv[2] == '0') {
        initial_solution = createGreedySolutionLW(num_items, items, max_weight);
        sol = calculateProfit(initial_solution, items, penalty, max_weight);
        cout << "Greedy solution with less weight: " << sol.items << " - "
             << sol.value << endl;
    } else if (*argv[2] == '1') {
        initial_solution = createGreedySolutionCB(num_items, items, max_weight);
        sol = calculateProfit(initial_solution, items, penalty, max_weight);
        cout << "Greedy solution with cost-benefit: " << sol.items << " - "
             << sol.value << endl;
    } else {
        printf("Error in parameter passing\n");
        return -1;
    }

    Solution best_solution = sol;
    vector<int> tabu_list(num_items, 0);
    int iterations = 0, best_change = 0, tabu_list_size = 2;

    while (best_change < 50) {
        cout << "Best: " << best_solution.items << " - " << best_solution.value
             << endl;
        sol = selectNewSolution(sol, best_solution, tabu_list, tabu_list_size,
                                penalty, items, num_items, max_weight);
        if (sol.value > best_solution.value) {
            best_solution = sol;
            best_change = 0;
        } else {
            best_change++;
        }
        iterations++;
    }

    t = clock() - t;
    printf("Execution Time: %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    free(items);
    inputFile.close();
    return 0;
}
