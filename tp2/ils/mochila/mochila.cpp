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

struct item {
    int profit, weight, position;
    float cost_benefit;
};

struct solution {
    string items;
    int value;
    int weight;
};

int get_random_number_from_interval(int begin, int end) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<> dist(begin, end);

    return dist(mersenne_engine);
}

item* read_knapsack(char** argv, int* n_items, int* wmax, int* penalty) {
    ifstream File(argv[1]);
    File >> *n_items;
    File >> *wmax;

    item* items = (item*)malloc(sizeof(item) * (*n_items));
    for (int i = 0; i < *n_items; i++) {
        File >> items[i].profit;
        File >> items[i].weight;
        items[i].cost_benefit =
            ((float)items[i].weight) / ((float)items[i].profit);
        items[i].position = i;
    }

    *penalty = 0;
    for (int i = 0; i < *n_items; i++) {
        *penalty += items[i].weight;
    }
    File.close();
    return items;
}

string create_solution(int n_items) {
    string s = "";
    for (int i = 0; i < n_items; i++) {
        s += '0';
    }
    int idx = get_random_number_from_interval(0, n_items - 1);
    s[idx] = '1';
    return s;
}

bool is_solution_valid(string solution, item* items, int w_max) {
    int weight = 0;

    for (int i = 0; i < solution.length(); i++) {
        if (weight > w_max) {
            return false;
        }
        weight += (solution[i] - '0') * items[i].weight;
    }
    if (weight > w_max) {
        return false;
    }
    return true;
}

solution calculate_profit(string sol, item* items, int penalty, int w_max) {
    solution s;
    s.items = sol;
    s.value = 0;
    s.weight = 0;
    for (int i = 0; i < sol.length(); i++) {
        s.value += (sol[items[i].position] - '0') * items[i].profit;
        s.weight += (sol[items[i].position] - '0') * items[i].weight;
    }
    s.value = s.value - (penalty * max(0, (s.weight - w_max)));
    return s;
}

solution generate_neighbor(solution sol, item* items, int position_switch,
                           int penalty, int w_max) {
    string s = sol.items;
    s[position_switch] = (s[position_switch] == '0') ? '1' : '0';
    return calculate_profit(s, items, penalty, w_max);
}

solution local_search(int n_items, item* items, int w_max, solution sol,
                      int penalty) {
    for (int i = 0; i < 2; i++) {
        solution neighbor;
        neighbor.items = sol.items;
        neighbor.value = sol.value;
        for (int j = 0; j < n_items; j++) {
            solution aux =
                generate_neighbor(neighbor, items, j, penalty, w_max);
            if (aux.value > neighbor.value) {
                neighbor = aux;
            }
        }
        if (neighbor.value > sol.value) {
            sol = neighbor;
            i = 0;
        }
    }
    return sol;
}

solution perturbation(int n_items, item* items, solution sol, int d, int w_max,
                      int penalty) {
    solution s;
    s.items = sol.items;
    s.value = sol.value;
    for (int i = 0; i < d; i++) {
        int pos = get_random_number_from_interval(0, n_items - 1);
        s.items[pos] = (s.items[pos] == '0') ? '1' : '0';
    }
    s = calculate_profit(s.items, items, penalty, w_max);

    return s;
}

int main(int argc, char** argv) {
    int n_items, wmax, penalty;

    item* items = read_knapsack(argv, &n_items, &wmax, &penalty);

    srand(time(NULL));
    clock_t t;

    t = clock();
    string s = create_solution(n_items);
    solution sol = calculate_profit(s, items, penalty, wmax);
    sol = local_search(n_items, items, wmax, sol, penalty);

    int max_iter = 50;
    int iter = 0;
    int d = 1;

    while (iter < max_iter) {
        iter++;
        solution s = perturbation(n_items, items, sol, d, wmax, penalty);
        s = local_search(n_items, items, wmax, s, penalty);
        if (!is_solution_valid(s.items, items, wmax)) {
            continue;
        }
        if (s.value > sol.value) {
            sol.value = s.value;
            sol.items = s.items;
            iter = 0;
            d = 1;
            printf("Valor atualizado = %d\n", sol.value);
        } else {
            d++;
        }
    }

    printf("Valor final = %d\n", sol.value);

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    free(items);
    return 0;
}