#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;

//minha função para achar as permutações
void gera_permutacao_recursivo(std::vector<int>& v, int n, int idx) {
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
                gera_permutacao_recursivo(v, n, idx + 1);
                break;
            }
            v[idx] = i;
            gera_permutacao_recursivo(v, n, idx+1);
        }
    }
}

void gera_permutacao_minha(int n) {
    std::vector<int> v(n);
    gera_permutacao_recursivo(v, n, 0);
}

//função usando o método next_permutation()
//e um pouco mais rápida que minha implementação
void gera_permutacao(int n) {
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);
    do {
        for (const int x : v)
            std::cout << x << ' ';
        std::cout << '\n';
    } while ((std::next_permutation(v.begin(), v.end())) && (v[0] == 1)); 
    //como o vetor representa um ciclo para o PCV, não precisamos de todas as permutações do vetor
}


int main(){

    srand(time(NULL));
    clock_t t;

    int n;

    printf("\nDigite um valor inteiro: ");
    scanf("%d", &n);
    t = clock();

    gera_permutacao(n);

    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t)/CLOCKS_PER_SEC);

    return 0;
}