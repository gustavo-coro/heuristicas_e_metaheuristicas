#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

void gera_permutacao_recursivo(std::vector<int>& v, int n, int idx, int** mat_adg) {
    if (idx == n) {
        for (const int& x : v)
            std::cout << x << ' ';
        std::cout << '\n';
        return;
    }
    for (int i = 1; i <= n; i++) {
        if (std::find(v.begin(), v.begin() + idx, i) == v.begin() + idx) {
            //como o vetor representa um ciclo para o PCV, não precisamos de todas as permutações do vetor
            if (idx == 0) {
                v[idx] = i;
                gera_permutacao_recursivo(v, n, idx + 1, mat_adg);
                break;
            }
            int ix = (v[idx - 1]) - 1;
            if (mat_adg[ix][i-1] != 0) {
                v[idx] = i;
                gera_permutacao_recursivo(v, n, idx + 1, mat_adg);
            }
        }
    }
}

void gera_permutacao(int n, int** mat_adg) {
    std::vector<int> v(n);
    gera_permutacao_recursivo(v, n, 0, mat_adg);
}


int main() {

    ifstream File("in.txt");

    int ver, ar;
    File >> ver;
    File >> ar;

    int** mat = (int**) malloc(sizeof(int*) * ver);
    for (int i = 0; i < ver; i++) {
        mat[i] = (int*) malloc(sizeof(int) * ver);
        for (int j = 0; j < ver; j++) {
            mat[i][j] = 0;
        }
    }

    for (int i = 0; i < ar; i++) {
        int v1, v2, p;
        File >> v1;
        File >> v2;
        File >> p;
        mat[v1 - 1][v2 - 1] = mat[v2 - 1][v1 - 1] = p;
    }

    File.close();

    srand(time(NULL));
    clock_t t;

    int n = ver;
    t = clock();

    gera_permutacao(n, mat);

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float) t) / CLOCKS_PER_SEC);

    return 0;
}