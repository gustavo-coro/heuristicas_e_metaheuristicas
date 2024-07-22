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

    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    std::uniform_int_distribution<int> dist{0, ver};

    std::vector<int> cities(ver);
    std::iota(std::begin(cities), std::end(cities), 1);
    std::shuffle(std::begin(cities), std::end(cities), mersenne_engine);
    return cities;
}

vector<int> gera_vizinho_qualquer(int ver, float **mat, solution atual)
{

    // First create an instance of an engine.
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    std::uniform_int_distribution<int> dist{0, ver - 1};

    int i = dist(mersenne_engine);
    std::uniform_int_distribution<int> new_dist{i, ver - 1};
    int j = new_dist(mersenne_engine);
    std::vector<int> vizinho = atual.v;

    int aux = vizinho[i];
    vizinho[i] = vizinho[j];
    vizinho[j] = aux;

    return vizinho;
}

float calcula_distancia(vector<int> solucao, int ver, float **mat_adg)
{
    float dist = 0.0;
    for (int i = 0; i < ver; i++)
    {
        if (i == (ver - 1))
        {
            dist += mat_adg[solucao[i] - 1][solucao[0] - 1];
            continue;
        }
        dist += mat_adg[solucao[i] - 1][solucao[i + 1] - 1];
    }
    return dist;
}

bool calcula_aceitacao(float temperatura, float dist_atual, float dist_vizinho)
{
    const double EulerConstant = std::exp(1.0);
    std::random_device rnd_device;
    // Specify the engine and distribution.
    std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    std::uniform_real_distribution<float> dist{0, 1};

    float r = dist(mersenne_engine);
    float to_pow = (dist_atual - dist_vizinho) / temperatura;
    float prob = pow(EulerConstant, to_pow);
    if (r < prob)
    {
        return true;
    }
    return false;
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Erro na passagem de parametros\n");
        return -1;
    }

    ifstream File(argv[1]);

    int termino = 100;
    float temperatura = 1000;
    float alfa = 0.98;

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

    int i = 0;
    while (temperatura > 0.000000000000001)
    {
        while (i < termino)
        {
            i++;
            solution vizinho;
            vizinho.v = gera_vizinho_qualquer(ver, mat, sol);
            vizinho.distancia = calcula_distancia(vizinho.v, ver, mat);
            if (vizinho.distancia < sol.distancia)
            {
                sol = vizinho;
                if (vizinho.distancia < best.distancia)
                {
                    best = vizinho;
                }
            }
            else if (calcula_aceitacao(temperatura, sol.distancia, vizinho.distancia))
            {
                sol = vizinho;
            }
        }
        printf("Distancia = %f\n", best.distancia);
        temperatura = temperatura * alfa;
        i = 0;
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