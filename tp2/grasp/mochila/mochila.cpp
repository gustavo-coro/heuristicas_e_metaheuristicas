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
    if (s[position_switch] == '0') {
        s[position_switch] = '1';
        return calculate_profit(s, items, penalty, w_max);
    }
    s[position_switch] = '0';
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

vector<int> get_items_by_limit(item* items, vector<int> lrc, float limit) {
    std::vector<int> it;
    for (int i = 0; i < lrc.size(); i++) {
        if (items[lrc[i]].weight <= limit) {
            it.push_back(lrc[i]);
        }
    }
    return it;
}

solution random_greedy_solution(int n_items, item* items, int w_max,
                                float alpha, int penalty) {
    // Start from a random item
    int start_item = get_random_number_from_interval(0, n_items - 1);

    // Vector to store the solution
    std::string sol = create_solution(n_items);

    // create LRC
    std::vector<int> lrc(n_items);
    iota(lrc.begin(), lrc.end(), 0);

    auto it = find(lrc.begin(), lrc.end(), start_item);

    if (it != lrc.end()) {
        lrc.erase(it);
    } else {
        perror("Erro ao remover item do vetor!\n");
        exit(1);
    }

    while (is_solution_valid(sol, items, w_max)) {
        item* min_weight = std::min_element(
            items, items + (n_items - 1), [](const item& lhs, const item& rhs) {
                return lhs.weight < rhs.weight;
            });
        item* max_weight = std::min_element(
            items, items + (n_items - 1), [](const item& lhs, const item& rhs) {
                return lhs.weight > rhs.weight;
            });
        float limit = min_weight->weight +
                      alpha * (max_weight->weight -
                               min_weight->weight);

        std::vector<int> items_selected = get_items_by_limit(items, lrc, limit);

        if (items_selected.size() == 0) {
            break;
        }

        int end_interval = items_selected.size() - 1;

        int next_item;
        if (end_interval >= 0) {
            next_item = items_selected[get_random_number_from_interval(
                0, end_interval)];
        }

        sol[next_item] = '1';

        auto it = find(lrc.begin(), lrc.end(), next_item);

        if (it != lrc.end()) {
            lrc.erase(it);
        } else {
            perror("Erro ao remover item do vetor!\n");
            exit(1);
        }
    }

    solution s = calculate_profit(sol, items, penalty, w_max);

    return s;
}

int main(int argc, char** argv) {
    int n_items, wmax, penalty;
    float alpha = 0.08;
    int max_iter = 100;

    item* items = read_knapsack(argv, &n_items, &wmax, &penalty);

    srand(time(NULL));
    clock_t t;

    t = clock();
    string s = create_solution(n_items);
    solution best = calculate_profit(s, items, penalty, wmax);

    for (int i = 0; i < max_iter; i++) {
        solution s =
            random_greedy_solution(n_items, items, wmax, alpha, penalty);
        s = local_search(n_items, items, wmax, s, penalty);
        if (s.value > best.value) {
            best.value = s.value;
            best.items = s.items;
            printf("Valor atualizado = %d\n", best.value);
        }
    }

    printf("Valor final = %d\n", best.value);

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    free(items);
    return 0;
}