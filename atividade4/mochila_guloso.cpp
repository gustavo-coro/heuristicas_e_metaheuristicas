#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

struct item {
    int profit, weight, position;
    float cost_benefit;
};

struct solution {
    string items;
    int value;
};


bool compare_item_cost_benefit(item i, item j) {return (i.cost_benefit > j.cost_benefit);}

bool compare_item_less_weight(item i, item j) {return (i.weight < j.weight);}

string create_solution(int n_items) {
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
        weight += (solution[items[i].position] - '0') * items[i].weight;
    }
    if (weight > w_max) {
        return false;
    }
    return true;
}

string create_solution_greedy_cb(int n_items, item* items, int w_max) {
    string solution = create_solution(n_items);
    std::sort(items, items+n_items, compare_item_cost_benefit);
    for (int i = 0; i < n_items; i++) {
        solution[items[i].position] = '1';
        if (!is_solution_valid(solution, items, w_max)) {
            solution[items[i].position] = '0';
        }
    }
    return solution;
}

string create_solution_greedy_lw(int n_items, item* items, int w_max) {
    string solution = create_solution(n_items);
    std::sort(items, items+n_items, compare_item_less_weight);
    for (int i = 0; i < n_items; i++) {
        solution[items[i].position] = '1';
        if (!is_solution_valid(solution, items, w_max)) {
            solution[items[i].position] = '0';
        }
    }
    return solution;
}

solution calculate_profit(string sol, item* items) {
    solution s;
    s.items = sol;
    s.value = 0;
    for (int i = 0; i < sol.length(); i++) {
        s.value += (sol[items[i].position] - '0') * items[i].profit;
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

int main(int argc, char** argv) {

    srand(time(NULL));
    clock_t t;

    // Aula2_Complementares/mochila_4_20
    // Aula2_Complementares/mochila_100_1000_1
    if (argc != 3) {
        printf("Erro na passagem de parametros\n");
        return -1;
    }
    ifstream File(argv[1]);

    int n_items, wmax;
    int parada = 100;
    File >> n_items;
    File >> wmax;

    item* items = (item*) malloc(sizeof(item) * n_items);
    for (int i = 0; i < n_items; i++) {
        File >> items[i].profit;
        File >> items[i].weight;
        items[i].cost_benefit = ((float) items[i].weight) / ((float) items[i].profit);
        items[i].position = i;
    }

    t = clock();

    string s;
    solution sol;

    if (*argv[2] == '0') {
        s = create_solution_greedy_lw(n_items, items, wmax);
        sol = calculate_profit(s, items);
        cout << "Greedy solution with less weight: ";
        cout << sol.items << " - ";
        printf("%d\n", sol.value);
    } else if (*argv[2] == '1') {
        s = create_solution_greedy_cb(n_items, items, wmax);
        sol = calculate_profit(s, items);
        cout << "Greedy solution with cost-benefit: ";
        cout << sol.items << " - ";
        printf("%d\n", sol.value);
    } else {
        printf("Erro na passagem de parametros\n");
        return -1;
    }
            
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

    printf("Tempo Execucao:  %f seconds.\n", ((float) t) / CLOCKS_PER_SEC);

    free(items);
    File.close();
    return 0;
}