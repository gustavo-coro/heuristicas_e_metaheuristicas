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

// Compilar com sanitizer: g++ atividade8.cpp -o atividade8 -fsanitize=address -static-libasan -g

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

int get_random_number_from_interval (int begin, int end) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<> dist(begin, end);

    return dist(mersenne_engine);
}

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
    while (iter_sem_melhora < 2)
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

solution perturbation(solution sol, int d, int ver, float **mat) {
    solution s;
    s.v = sol.v;
    s.distancia = sol.distancia;
    for (int i = 0; i < d; i++) {
        int pos1 = get_random_number_from_interval(0, ver - 1);
        int pos2 = get_random_number_from_interval(0, ver - 1);
        while (pos1 == pos2) {
            pos2 = get_random_number_from_interval(0, ver - 1);
        }
        int temp = s.v[pos1];
        s.v[pos1] = s.v[pos2];
        s.v[pos2] = temp;
    }
    s.distancia = calcula_distancia(s.v, ver, mat);

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

    sol = busca_local(sol, ver, mat);

    int max_iter = 25;
    int iter = 0;
    int d = 1;

    while (iter < max_iter) {
        iter++;
        solution s = perturbation(sol, d, ver, mat);
        s = busca_local(s, ver, mat);
        if (s.distancia < sol.distancia) {
            sol.v = s.v;
            sol.distancia = s.distancia;
            iter = 0;
            d = 1;
            printf("Distancia = %f\n", sol.distancia);
        } else {
            d++;
        }
    }


    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    printf("Distancia = %f\n", sol.distancia);
    printf("Solucao = ");
    for (int i = 0; i < ver; i++)
    {
        printf("[%d] ", sol.v[i]);
    }
    printf("\n");

    for (int i = 0; i < ver; i++)
    {
        free(mat[i]);
    }
    free(mat);

    return 0;
}