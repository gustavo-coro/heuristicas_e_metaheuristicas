#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <bits/stdc++.h>

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

bool is_solution_valid(vector<int> solucao, int ver, float **mat_adg)
{
    for (int i = 0; i < ver; i++)
    {
        if (i == (ver - 1))
        {
            if (mat_adg[solucao[i] - 1][solucao[0] - 1] == 0.0)
            {
                return false;
            }
            continue;
        }
        if (mat_adg[solucao[i] - 1][solucao[i + 1] - 1] == 0.0)
        {
            return false;
        }
    }
    return true;
}

vector<int> gera_solucao_inicial(int ver, float **mat_adg)
{
    std::vector<int> v(ver);
    std::iota(v.begin(), v.end(), 1);
    do
    {
        if (is_solution_valid(v, ver, mat_adg))
        {
            return v;
        }
    } while ((std::next_permutation(v.begin(), v.end())) && (v[0] == 1));

    for (int i = 0; i < ver; i++)
    {
        v.push_back(0);
    }
    return v;
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

vector<int> gera_vizinho(vector<int> solucao, int ver, int f_idx, int s_idx)
{
    vector<int> vizinho = solucao;
    int aux = vizinho[f_idx];
    vizinho[f_idx] = vizinho[s_idx];
    vizinho[s_idx] = aux;
    return vizinho;
}

int main(int argc, char **argv)
{

    ifstream File(argv[1]);

    int parada = 50;
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

    vector<int> solucao = gera_solucao_inicial(ver, mat);
    for (int i = 0; i < ver; i++)
    {
        if (solucao[i] == 0)
        {
            perror("Solucao nao existe!\n");
            for (int i = 0; i < ver; i++)
            {
                free(mat[i]);
            }
            free(mat);

            File.close();
            return 0;
        }
    }
    float dist = calcula_distancia(solucao, ver, mat);
    solution sol;
    sol.distancia = dist;
    sol.v = solucao;

    for (int i = 0; i < parada; i++)
    {
        printf("%lf\n", sol.distancia);
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
                while (!is_solution_valid(v, ver, mat))
                {
                    v = gera_vizinho(sol.v, ver, i, j);
                }
                float dist_v = calcula_distancia(v, ver, mat);
                if (dist_v < vizinho.distancia)
                {
                    vizinho.v = v;
                    vizinho.distancia = dist_v;
                }
            }
        }
        sol = vizinho;
    }
    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);

    for (int j = 0; j < ver; j++)
    {
        printf("%d ", sol.v[j]);
    }
    printf("- Dist = %lf\n", sol.distancia);

    for (int i = 0; i < ver; i++)
    {
        free(mat[i]);
    }
    free(mat);

    File.close();
    return 0;
}