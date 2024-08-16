#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <math.h>

using namespace std;

struct item
{
    int profit, weight, position;
    float cost_benefit;
};

struct solution
{
    string items;
    int value;
    int weight;
};

bool compare_item_cost_benefit(item i, item j) { return (i.cost_benefit > j.cost_benefit); }

bool compare_item_less_weight(item i, item j) { return (i.weight < j.weight); }

string create_solution(int n_items)
{
    string s = "";
    for (int i = 0; i < n_items; i++)
    {
        s += '0';
    }
    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    std::uniform_int_distribution<int> dist{0, n_items - 1};

    int i = dist(mersenne_engine);
    s[i] = '1';
    return s;
}

bool is_solution_valid(string solution, item *items, int w_max)
{
    int weight = 0;

    for (int i = 0; i < solution.length(); i++)
    {
        if (weight > w_max)
        {
            return false;
        }
        weight += (solution[items[i].position] - '0') * items[i].weight;
    }
    if (weight > w_max)
    {
        return false;
    }
    return true;
}

string create_solution_greedy_cb(int n_items, item *items, int w_max)
{
    string solution = create_solution(n_items);
    std::sort(items, items + n_items, compare_item_cost_benefit);
    for (int i = 0; i < n_items; i++)
    {
        solution[items[i].position] = '1';
        if (!is_solution_valid(solution, items, w_max))
        {
            solution[items[i].position] = '0';
        }
    }
    return solution;
}

string create_solution_greedy_lw(int n_items, item *items, int w_max)
{
    string solution = create_solution(n_items);
    std::sort(items, items + n_items, compare_item_less_weight);
    for (int i = 0; i < n_items; i++)
    {
        solution[items[i].position] = '1';
        if (!is_solution_valid(solution, items, w_max))
        {
            solution[items[i].position] = '0';
        }
    }
    return solution;
}

solution calculate_profit(string sol, item *items, int penalty, int w_max)
{
    solution s;
    s.items = sol;
    s.value = 0;
    s.weight = 0;
    for (int i = 0; i < sol.length(); i++)
    {
        s.value += (sol[items[i].position] - '0') * items[i].profit;
        s.weight += (sol[items[i].position] - '0') * items[i].weight;
    }
    s.value = s.value - (penalty * max(0, (s.weight - w_max)));
    return s;
}

solution generate_neighbor(solution sol, item *items, int position_switch, int penalty, int w_max)
{
    string s = sol.items;
    if (s[position_switch] == '0')
    {
        s[position_switch] = '1';
        return calculate_profit(s, items, penalty, w_max);
    }
    s[position_switch] = '0';
    return calculate_profit(s, items, penalty, w_max);
}

solution select_new_solution(solution s, solution best, vector<int> &tabu, int tabu_size, int penalty, item *items, int n_items, int w_max)
{
    vector<solution> neighborhood(n_items);
    for (int i = 0; i < n_items; i++)
    {
        solution n = generate_neighbor(s, items, i, penalty, w_max);
        neighborhood[i] = n;
    }
    solution n;
    int i, n_index;

    if (tabu_size == n_items)
    {
        int menor = 0;
        for (i = 0; i < n_items; i++)
        {
            if (tabu[menor] == 0) {
                break;
            }
            if (tabu[i] < menor) {
                menor = i;
            }
        }
        tabu[menor] = 0;
    }

    for (i = 0; i < n_items; i++)
    {
        if ((tabu[i] == 0) || ((neighborhood[i].value) > best.value))
        {
            n = neighborhood[i];
            n_index = i;
            break;
        }
    }
    for (; i < n_items; i++)
    {
        if ((neighborhood[i].value > n.value) && ((tabu[i] == 0) || (neighborhood[i].value) > best.value))
        {
            n = neighborhood[i];
            n_index = i;
        }
    }
    for (i = 0; i < n_items; i++)
    {
        if (tabu[i] != 0)
        {
            tabu[i]--;
        }
    }
    tabu[n_index] = tabu_size;
    return n;
}

int main(int argc, char **argv)
{

    srand(time(NULL));
    clock_t t;

    // Aula2_Complementares/mochila_4_20
    // Aula2_Complementares/mochila_100_1000_1
    if (argc != 3)
    {
        printf("Erro na passagem de parametros\n");
        return -1;
    }
    ifstream File(argv[1]);

    int n_items, wmax;
    int parada = 100;
    File >> n_items;
    File >> wmax;

    item *items = (item *)malloc(sizeof(item) * n_items);
    for (int i = 0; i < n_items; i++)
    {
        File >> items[i].profit;
        File >> items[i].weight;
        items[i].cost_benefit = ((float)items[i].weight) / ((float)items[i].profit);
        items[i].position = i;
    }

    int penalty = 0;
    for (int i = 0; i < n_items; i++)
    {
        penalty += items[i].weight;
    }

    string s;
    solution sol;

    t = clock();

    if (*argv[2] == '0')
    {
        s = create_solution_greedy_lw(n_items, items, wmax);
        sol = calculate_profit(s, items, penalty, wmax);
        cout << "Greedy solution with less weight: ";
        cout << sol.items << " - ";
        printf("%d\n", sol.value);
    }
    else if (*argv[2] == '1')
    {
        s = create_solution_greedy_cb(n_items, items, wmax);
        sol = calculate_profit(s, items, penalty, wmax);
        cout << "Greedy solution with cost-benefit: ";
        cout << sol.items << " - ";
        printf("%d\n", sol.value);
    }
    else
    {
        printf("Erro na passagem de parametros\n");
        return -1;
    }

    solution best = sol;
    std::vector<int> tabu(n_items, 0);
    int iter = 0, best_change = 0, tabu_size = 2;
    while (best_change < 50)
    {
        cout << "Best: ";
        cout << best.items << " - ";
        printf("%d\n", best.value);
        sol = select_new_solution(sol, best, tabu, tabu_size, penalty, items, n_items, wmax);
        if (sol.value > best.value)
        {
            best = sol;
            best_change = 0;
        }
        else
        {
            best_change += 1;
        }
        iter += 1;
    }

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    free(items);
    File.close();
    return 0;
}