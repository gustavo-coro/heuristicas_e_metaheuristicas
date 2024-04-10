#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

void gera_combinacao_recursivo(char* v, int size, int idc) {
    if (idc == 0) {
        for (int i = 0; i < size; i++) {
            printf("%c", v[i]);
        }
        printf("\n");
        v[0] = '1';
        for (int i = 0; i < size; i++) {
            printf("%c", v[i]);
        }
        printf("\n");
        return;
    }
    char* new_v = (char*) malloc(sizeof(char) * size);
    for (int i = 0; i < size; i++) {
        new_v[i] = v[i];
    } 
    gera_combinacao_recursivo(new_v, size, idc-1);
    free(new_v);
    new_v = (char*) malloc(sizeof(char) * size);
    for (int i = 0; i < size; i++) {
        new_v[i] = v[i];
    } 
    new_v[idc] = '1';
    gera_combinacao_recursivo(new_v, size, idc-1);
    free(new_v);
}

void gera_combinacao_binaria(char* v, int size) {
    for (int i = 0; i < size; i++) {
        v[i] = '0';
    }
    gera_combinacao_recursivo(v, size, size-1);

}

int main(){

    srand(time(NULL));
    clock_t t;

    int n;
    char* v;

    printf("\nDigite um valor inteiro: ");
    scanf("%d", &n);

    v = (char*) malloc(sizeof(char) * n);
    t = clock();
    gera_combinacao_binaria(v, n);
    t = clock() - t;

    printf("Tempo Execucao:  %f seconds.\n", ((float)t)/CLOCKS_PER_SEC);

    free(v);
    return 0;
}