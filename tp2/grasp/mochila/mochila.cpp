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
    int profit, weight;
};

struct solution {
    string items;
    int value;
};

int get_random_from_interval(int begin, int end) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<> dist(begin, end);

    return dist(mersenne_engine);
}

item* read_knapsack(char** argv, int* n_items, int* wmax) {
    ifstream File(argv[1]);
    File >> *n_items;
    File >> *wmax;

    item* items = (item*)malloc(sizeof(item) * (*n_items));
    for (int i = 0; i < *n_items; i++) {
        File >> items[i].profit;
        File >> items[i].weight;
    }

    return items;
}

string create_solution(int n_items, item* items, int w_max) {
    string s = "";
    for (int i = 0; i < n_items; i++) {
        s += '0';
    }
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

solution calculate_profit(string sol, item* items) {
    solution s;
    s.items = sol;
    s.value = 0;
    for (int i = 0; i < sol.length(); i++) {
        s.value += (sol[i] - '0') * items[i].profit;
    }
    return s;
}

solution generate_neighbor(solution sol, item* items, int position_switch) {
    solution neighbor;
    neighbor.items = sol.items;
    neighbor.value = sol.value;
    if (neighbor.items[position_switch] == '0') {
        neighbor.items[position_switch] = '1';
        neighbor.value += items[position_switch].profit;
        return neighbor;
    }
    neighbor.items[position_switch] = '0';
    neighbor.value -= items[position_switch].profit;
    return neighbor;
}

solution random_greedy_solution(int n_items, item* items, int w_max, float alpha) {



 }

int main(int argc, char** argv) {
    int n_items, wmax;
    int parada = 100;

    item* items = read_knapsack(argv, &n_items, &wmax);

    srand(time(NULL));
    clock_t t;

    t = clock();
    string s = create_solution(n_items, items, wmax);
    solution sol = calculate_profit(s, items);
    for (int i = 0; i < parada; i++) {
        cout << sol.items << " - ";
        printf("%d\n", sol.value);
        solution neighbor;
        neighbor.items = sol.items;
        neighbor.value = sol.value;
        for (int j = 0; j < s.length(); j++) {
            solution aux = generate_neighbor(sol, items, j);
            if (!is_solution_valid(aux.items, items, wmax)) {
                continue;
            }
            if (aux.value > neighbor.value) {
                neighbor = aux;
                break;
            }
        }
        sol = neighbor;
    }

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    free(items);
    return 0;
}