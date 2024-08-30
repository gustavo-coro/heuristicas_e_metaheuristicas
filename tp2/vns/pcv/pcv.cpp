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

struct cidade {
    int id;
    int x;
    int y;
};

struct solution {
    std::vector<int> v;
    float distancia;
};

void read_graph(char **argv, int *v, float ***m) {
    ifstream File(argv[1]);

    int ver;
    File >> ver;
    *v = ver;

    vector<cidade> cidades;

    float **mat = (float **)malloc(sizeof(float *) * ver);
    for (int i = 0; i < ver; i++) {
        mat[i] = (float *)malloc(sizeof(float) * ver);
        for (int j = 0; j < ver; j++) {
            mat[i][j] = 0.0;
        }
    }

    for (int i = 0; i < ver; i++) {
        cidade c;
        File >> c.id;
        File >> c.x;
        File >> c.y;
        cidades.push_back(c);
    }

    for (int i = 0; i < ver; i++) {
        int idx_f = cidades[i].id - 1;
        for (int j = 0; j < ver; j++) {
            int idx_s = cidades[j].id - 1;
            int xd = cidades[i].x - cidades[j].x;
            int yd = cidades[i].y - cidades[j].y;
            mat[idx_f][idx_s] = sqrt(xd * xd + yd * yd);
        }
    }

    File.close();

    *m = mat;
}

int get_random_from_interval(int begin, int end) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<> dist(begin, end);

    return dist(mersenne_engine);
}

float calcula_distancia(vector<int> solucao, int ver, float **mat_adg) {
    float dist = 0.0;
    for (int i = 0; i < ver; i++) {
        if (i == (ver - 1)) {
            dist += mat_adg[solucao[i]][solucao[0]];
            continue;
        }
        dist += mat_adg[solucao[i]][solucao[i + 1]];
    }
    return dist;
}

vector<int> gera_solucao_aleatoria(int ver, float **mat_adg) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<> dist(0, ver - 1);

    std::vector<int> cities(ver);
    std::iota(std::begin(cities), std::end(cities), 0);
    std::shuffle(std::begin(cities), std::end(cities), mersenne_engine);
    return cities;
}

vector<int> gera_vizinho_swap_two(vector<int> solucao, int ver, int f_idx,
                                  int s_idx) {
    vector<int> vizinho = solucao;
    std::swap(vizinho[f_idx], vizinho[s_idx]);
    return vizinho;
}

vector<int> gera_vizinho_opt(vector<int> solucao, int ver, int i, int j) {
    vector<int> vizinho = solucao;
    std::reverse(vizinho.begin() + i, vizinho.begin() + j);
    return vizinho;
}

vector<int> gera_vizinho_insert(vector<int> solucao, int ver, int i, int j) {
    vector<int> vizinho = solucao;
    int city = vizinho[i];
    vizinho.erase(vizinho.begin() + i);
    vizinho.insert(vizinho.begin() + j, city);
    return vizinho;
}

solution busca_local_swap(solution sol_inicial, int ver, float **mat) {
    solution sol = sol_inicial;
    bool improv = true;
    while (improv) {
        solution vizinho = sol;
        for (int j = 0; j < ver; j++) {
            for (int k = j + 1; k < ver; k++) {
                if (k == j) {
                    continue;
                }

                vector<int> v = gera_vizinho_swap_two(sol.v, ver, j, k);
                float dist_v = calcula_distancia(v, ver, mat);

                if (dist_v < vizinho.distancia) {
                    vizinho.v = v;
                    vizinho.distancia = dist_v;
                }
            }
        }
        if (vizinho.distancia < sol.distancia) {
            sol = vizinho;
        } else {
            improv = false;
        }
    }

    return sol;
}

solution busca_local_opt(solution sol_inicial, int ver, float **mat) {
    solution sol = sol_inicial;
    bool improv = true;
    while (improv) {
        solution vizinho = sol;
        for (int j = 1; j < ver - 1; j++) {
            for (int k = j + 1; k < ver; k++) {
                vector<int> v = gera_vizinho_opt(sol.v, ver, j, k);
                float dist_v = calcula_distancia(v, ver, mat);

                if (dist_v < vizinho.distancia) {
                    vizinho.v = v;
                    vizinho.distancia = dist_v;
                }
            }
        }
        if (vizinho.distancia < sol.distancia) {
            sol = vizinho;
        } else {
            improv = false;
        }
    }

    return sol;
}

solution busca_local_insert(solution sol_inicial, int ver, float **mat) {
    solution sol = sol_inicial;
    bool improv = true;
    while (improv) {
        solution vizinho = sol;
        for (int j = 0; j < ver; j++) {
            for (int k = 0; k < ver; k++) {
                if (k == j) {
                    continue;
                }

                vector<int> v = gera_vizinho_insert(sol.v, ver, j, k);
                float dist_v = calcula_distancia(v, ver, mat);

                if (dist_v < vizinho.distancia) {
                    vizinho.v = v;
                    vizinho.distancia = dist_v;
                }
            }
        }
        if (vizinho.distancia < sol.distancia) {
            sol = vizinho;
        } else {
            improv = false;
        }
    }

    return sol;
}

solution busca_local(solution sol_inicial, int ver, float **mat,
                     int vizinhaca) {
    solution sol;
    switch (vizinhaca) {
        case 0: {
            sol = busca_local_swap(sol_inicial, ver, mat);
            break;
        }
        case 1: {
            sol = busca_local_opt(sol_inicial, ver, mat);
            break;
        }
        case 2: {
            sol = busca_local_insert(sol_inicial, ver, mat);
            break;
        }
    }

    return sol;
}

solution perturbation(solution sol, int ver, float **mat, int vizinhaca) {
    int d = 2;
    solution s;
    s.v = sol.v;
    s.distancia = sol.distancia;
    for (int i = 0; i < d; i++) {
        switch (vizinhaca) {
            case 0: {
                int pos1 = get_random_from_interval(0, ver - 1);
                int pos2 = get_random_from_interval(0, ver - 1);
                while (pos1 == pos2) {
                    pos2 = get_random_from_interval(0, ver - 1);
                }
                s.v = gera_vizinho_swap_two(s.v, ver, pos1, pos2);
                break;
            }
            case 1: {
                int pos1 = get_random_from_interval(1, ver - 2);
                int pos2 = get_random_from_interval(pos1, ver - 1);
                while (pos1 == pos2) {
                    pos2 = get_random_from_interval(pos1, ver - 1);
                }
                s.v = gera_vizinho_opt(s.v, ver, pos1, pos2);
                break;
            }
            case 2: {
                int pos1 = get_random_from_interval(0, ver - 1);
                int pos2 = get_random_from_interval(0, ver - 1);
                while (pos1 == pos2) {
                    pos2 = get_random_from_interval(0, ver - 1);
                }
                s.v = gera_vizinho_insert(s.v, ver, pos1, pos2);
                break;
            }
        }
    }
    s.distancia = calcula_distancia(s.v, ver, mat);

    return s;
}

int main(int argc, char **argv) {
    int ver;
    float **mat;
    solution sol;
    int num_vizinhacas = 3;
    int vizinhaca = 0;
    int iter = 0;
    int max_iter = stoi(argv[2]);

    read_graph(argv, &ver, &mat);

    srand(time(NULL));
    clock_t t;
    t = clock();

    vector<int> solucao = gera_solucao_aleatoria(ver, mat);
    float dist = calcula_distancia(solucao, ver, mat);
    sol.distancia = dist;
    sol.v = solucao;

    printf("Distancia = %f\n", sol.distancia);

    while (iter < max_iter) {
        solution sol_iter;
        sol_iter.distancia = sol.distancia;
        sol_iter.v = sol.v;

        vizinhaca = 0;

        while (vizinhaca < num_vizinhacas) {
            solution s = perturbation(sol_iter, ver, mat, vizinhaca);
            s = busca_local(s, ver, mat, vizinhaca);

            if (s.distancia < sol_iter.distancia) {
                sol_iter.distancia = s.distancia;
                sol_iter.v = s.v;
                vizinhaca = 0;
                printf("Distancia = %f\n", sol_iter.distancia);
            } else {
                vizinhaca++;
            }
        }
        if (sol_iter.distancia < sol.distancia) {
            sol.distancia = sol_iter.distancia;
            sol.v = sol_iter.v;
            iter = 0;
        } else {
            iter++;
        }
    }

    t = clock() - t;

    printf("Tempo Execucao: %f seconds. \n", ((float)t) / CLOCKS_PER_SEC);

    printf("Distancia = %f\n", sol.distancia);
    printf("Solucao = ");
    for (int i = 0; i < ver; i++) {
        printf("[%d] ", sol.v[i]);
    }
    printf("\n");

    for (int i = 0; i < ver; i++) {
        free(mat[i]);
    }
    free(mat);
    return 1;
}
