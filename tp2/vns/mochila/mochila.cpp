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

bool compare_item_cost_benefit(item i, item j) {
    return (i.cost_benefit > j.cost_benefit);
}

bool compare_item_less_weight(item i, item j) { return (i.weight < j.weight); }

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
        weight += (solution[items[i].position] - '0') * items[i].weight;
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

solution flip_item(solution sol, item* items, int position_switch, int penalty,
                   int w_max) {
    string s = sol.items;
    s[position_switch] = (s[position_switch] == '0') ? '1' : '0';
    return calculate_profit(s, items, penalty, w_max);
}

solution swap_two_items(solution sol, item* items, int f_position,
                        int s_position, int penalty, int w_max) {
    string s = sol.items;
    swap(s[f_position], s[s_position]);
    return calculate_profit(s, items, penalty, w_max);
}

solution shift_items(solution sol, item* items, int i, int block_size,
                     int penalty, int w_max) {
    string s = sol.items;
    rotate(s.begin() + i, s.begin() + i + 1, s.begin() + i + block_size);
    return calculate_profit(s, items, penalty, w_max);
}

solution local_search_flip(int n_items, item* items, int w_max, solution sol,
                           int penalty) {
    bool improv = true;
    while (improv) {
        solution neighbor;
        neighbor.items = sol.items;
        neighbor.value = sol.value;
        neighbor.weight = sol.weight;
        for (int j = 0; j < n_items; j++) {
            solution aux = flip_item(sol, items, j, penalty, w_max);
            if (aux.value > neighbor.value) {
                neighbor = aux;
            }
        }
        if (neighbor.value > sol.value) {
            sol = neighbor;
        } else {
            improv = false;
        }
    }
    return sol;
}

solution local_search_swap(int n_items, item* items, int w_max, solution sol,
                           int penalty) {
    bool improv = true;
    while (improv) {
        solution neighbor;
        neighbor.items = sol.items;
        neighbor.value = sol.value;
        neighbor.weight = sol.weight;
        for (int i = 0; i < n_items; i++) {
            for (int j = 0; j < n_items; j++) {
                solution aux =
                    swap_two_items(neighbor, items, i, j, penalty, w_max);
                if (aux.value > neighbor.value) {
                    neighbor = aux;
                }
            }
        }
        if (neighbor.value > sol.value) {
            sol = neighbor;
        } else {
            improv = false;
        }
    }
    return sol;
}

solution local_search_shift(int n_items, item* items, int w_max, solution sol,
                            int penalty) {
    bool improv = true;
    while (improv) {
        solution neighbor;
        neighbor.items = sol.items;
        neighbor.value = sol.value;
        neighbor.weight = sol.weight;
        for (int blockSize = 0; blockSize <= n_items / 2; blockSize++) {
            for (int i = 0; i + blockSize < n_items; i++) {
                solution aux =
                    shift_items(neighbor, items, i, blockSize, penalty, w_max);
                if (aux.value > neighbor.value) {
                    neighbor = aux;
                }
            }
        }
        if (neighbor.value > sol.value) {
            sol = neighbor;
        } else {
            improv = false;
        }
    }
    return sol;
}

solution local_search(int n_items, item* items, int w_max, solution sol,
                      int penalty, int neighborhood) {
    switch (neighborhood) {
        case 1: {
            sol = local_search_flip(n_items, items, w_max, sol, penalty);
            break;
        }
        case 2: {
            sol = local_search_swap(n_items, items, w_max, sol, penalty);
            break;
        }
        case 3: {
            sol = local_search_shift(n_items, items, w_max, sol, penalty);
            break;
        }
    }

    return sol;
}

solution perturbation(int n_items, item* items, solution sol, int w_max,
                      int penalty, int neighborhood) {
    int d = 2;
    solution s;
    s.items = sol.items;
    s.value = sol.value;
    s.weight = sol.weight;
    for (int i = 0; i < d; i++) {
        switch (neighborhood) {
            case 1: {
                int fir = get_random_number_from_interval(0, n_items - 1);
                s = flip_item(s, items, fir, penalty, w_max);
                break;
            }
            case 2: {
                int fir = get_random_number_from_interval(0, n_items - 1);
                int sec = get_random_number_from_interval(0, n_items - 1);
                while (fir == sec) {
                    sec = get_random_number_from_interval(0, n_items - 1);
                }
                s = swap_two_items(s, items, fir, sec, penalty, w_max);
                break;
            }
            case 3: {
                int block =
                    get_random_number_from_interval(0, (int)n_items / 2);
                int i = get_random_number_from_interval(0, n_items - block - 1);
                s = shift_items(s, items, i, block, penalty, w_max);
                break;
            }
        }
    }
    s = calculate_profit(s.items, items, penalty, w_max);

    return s;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    clock_t t;

    int n_items, wmax, penalty;
    item* items = read_knapsack(argv, &n_items, &wmax, &penalty);
    string s;
    solution sol;

    int num_vizinhacas = 3;
    int vizinhaca = 0;
    int iter = 0;
    int max_iter = stoi(argv[2]);

    t = clock();

    s = create_solution(n_items);
    sol = calculate_profit(s, items, penalty, wmax);

    while (iter < max_iter) {
        solution sol_iter;
        sol_iter.items = sol.items;
        sol_iter.value = sol.value;
        sol_iter.weight = sol.weight;

        vizinhaca = 0;

        while (vizinhaca < num_vizinhacas) {
            solution s = perturbation(n_items, items, sol_iter, wmax, penalty,
                                      vizinhaca);
            s = local_search(n_items, items, wmax, s, penalty, vizinhaca);

            if (s.value > sol_iter.value) {
                sol_iter.items = s.items;
                sol_iter.value = s.value;
                sol_iter.weight = s.weight;
                vizinhaca = 0;
                printf("Distancia = %d\n", sol_iter.value);
            } else {
                vizinhaca++;
            }
        }

        if ((sol_iter.value > sol.value) &&
            (is_solution_valid(sol_iter.items, items, wmax))) {
            sol.items = sol_iter.items;
            sol.value = sol_iter.value;
            sol.weight = sol_iter.weight;
            iter = 0;
        } else {
            iter++;
        }
    }

    printf("Distancia final = %d\n", sol.value);

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    free(items);
    return 0;
}