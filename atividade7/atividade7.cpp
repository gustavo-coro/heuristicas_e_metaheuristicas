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

// Compilar com sanitizer: sag++ atividade5.cpp -o atividade5 -fsanitize=address -static-libasan -g

using namespace std;

struct cidade
{
    int id;
    int x;
    int y;
};

struct solution
{
    std::vector<int> v;
    float distancia;
};

vector<int> gera_solucao_aleatoria(int ver, float **mat_adg)
{
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<int> dist{0, ver - 1};

    std::vector<int> cities(ver);
    std::iota(std::begin(cities), std::end(cities), 0);
    std::shuffle(std::begin(cities), std::end(cities), mersenne_engine);
    return cities;
}

vector<int> gera_vizinho(vector<int> solucao, int ver, int f_idx, int s_idx)
{
    vector<int> vizinho = solucao;
    int aux = vizinho[f_idx];
    vizinho[f_idx] = vizinho[s_idx];
    vizinho[s_idx] = aux;
    return vizinho;
}

float calcula_distancia(vector<int> solucao, int ver, float **mat_adg)
{
    float dist = 0.0;
    for (int i = 0; i < ver; i++)
    {
        if (i == (ver - 1))
        {
            dist += mat_adg[solucao[i]][solucao[0]];
            continue;
        }
        dist += mat_adg[solucao[i]][solucao[i + 1]];
    }
    return dist;
}
solution busca_local(solution sol_inicial, int ver, float **mat)
{
    solution sol = sol_inicial;
    int iter_sem_melhora = 0;
    while (iter_sem_melhora < 25)
    {
        solution vizinho = sol;
        for (int j = 0; j < ver; j++)
        {
            for (int k = 0; k < ver; k++)
            {
                if (k == j)
                {
                    continue;
                }

                vector<int> v = gera_vizinho(sol.v, ver, j, k);
                float dist_v = calcula_distancia(v, ver, mat);

                if (dist_v < vizinho.distancia)
                {
                    vizinho.v = v;
                    vizinho.distancia = dist_v;
                    iter_sem_melhora = 0;
                }
            }
        }
        sol = vizinho;
        iter_sem_melhora++;
    }

    return sol;
}

vector<float> get_distances_from_cities_in_lrc(int city, float **distance_matrix, vector<int> lrc)
{
    std::vector<float> distances;

    for (int i = 0; i < lrc.size(); i++)
    {
        distances.push_back(distance_matrix[city][lrc[i]]);
    }

    return distances;
}

vector<int> get_cities_lesser_than_limit(vector<float> distances, vector<int> lrc, float limit)
{
    std::vector<int> cities;
    for (int i = 0; i < lrc.size(); i++)
    {
        if (distances[i] <= limit)
        {
            cities.push_back(lrc[i]);
        }
    }
    return cities;
}

int get_random_number_from_interval (int begin, int end) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<> dist(begin, end);

    return dist(mersenne_engine);
}

solution random_greedy_solution(int num_cities, float **distance_matrix, float alpha)
{
    // Start from a random city
    int start_city = get_random_number_from_interval(0, num_cities - 1);

    // Vector to store the solution
    std::vector<int> sol;
    sol.push_back(start_city);

    // create LRC
    std::vector<int> lrc(num_cities);
    iota(lrc.begin(), lrc.end(), 0);; 

    auto it = find(lrc.begin(), lrc.end(), start_city);

    if (it != lrc.end())
    {
        lrc.erase(it);
    }
    else
    {
        perror("Erro ao remover item do vetor!\n");
        exit(1);
    }

    while (sol.size() < num_cities)
    {
        int last_city = sol.back();
        std::vector<float> distances = get_distances_from_cities_in_lrc(last_city, distance_matrix, lrc);

        float min_distance = *min_element(distances.begin(), distances.end());
        float max_distance = *max_element(distances.begin(), distances.end());
        float limit = min_distance + alpha * (max_distance - min_distance);

        std::vector<int> cities = get_cities_lesser_than_limit(distances, lrc, limit);
        int end_interval = cities.size() - 1;

        int next_city;
        if (end_interval >= 0) {
            next_city = cities[get_random_number_from_interval(0, end_interval)];
        }

        sol.push_back(next_city);

        auto it = find(lrc.begin(), lrc.end(), next_city);

        if (it != lrc.end())
        {
            lrc.erase(it);
        }
        else
        {
            perror("Erro ao remover item do vetor!\n");
            exit(1);
        }
    }

    solution s;
    s.v = sol;
    s.distancia = calcula_distancia(s.v, num_cities, distance_matrix);

    return s;
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Erro na passagem de parametros\n");
        return -1;
    }

    ifstream File(argv[1]);

    int ver;
    File >> ver;

    vector<cidade> cidades;

    float **mat = (float **)malloc(sizeof(float *) * ver);
    for (int i = 0; i < ver; i++)
    {
        mat[i] = (float *)malloc(sizeof(float) * ver);
        for (int j = 0; j < ver; j++)
        {
            mat[i][j] = 0.0;
        }
    }

    for (int i = 0; i < ver; i++)
    {
        cidade c;
        File >> c.id;
        File >> c.x;
        File >> c.y;
        cidades.push_back(c);
    }

    for (int i = 0; i < ver; i++)
    {
        int idx_f = cidades[i].id - 1;
        for (int j = 0; j < ver; j++)
        {
            int idx_s = cidades[j].id - 1;
            int xd = cidades[i].x - cidades[j].x;
            int yd = cidades[i].y - cidades[j].y;
            mat[idx_f][idx_s] = sqrt(xd * xd + yd * yd);
        }
    }

    File.close();

    srand(time(NULL));
    clock_t t;
    t = clock();

    vector<int> solucao = gera_solucao_aleatoria(ver, mat);

    float dist = calcula_distancia(solucao, ver, mat);
    solution sol;
    sol.distancia = dist;
    sol.v = solucao;

    printf("Distancia = %f\n", sol.distancia);
    printf("Solucao = ");
    for (int i = 0; i < ver; i++)
    {
        printf("[%d] ", sol.v[i]);
    }
    printf("\n");

    solution best = sol;

    float alfa = 0.05;
    int max_iter = 65;

    for (int i = 0; i < max_iter; i++)
    {
        solution s = random_greedy_solution(ver, mat, alfa);
        s = busca_local(s, ver, mat);
        if (s.distancia < best.distancia)
        {
            best.distancia = s.distancia;
            best.v = s.v;
            printf("Distancia atualizada = %f\n", best.distancia);
        }
    }

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    printf("Distancia = %f\n", best.distancia);
    printf("Solucao = ");
    for (int i = 0; i < ver; i++)
    {
        printf("[%d] ", best.v[i]);
    }
    printf("\n");

    for (int i = 0; i < ver; i++)
    {
        free(mat[i]);
    }
    free(mat);

    return 0;
}