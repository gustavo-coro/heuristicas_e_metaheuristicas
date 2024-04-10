#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct aresta {
    int ini, fim, peso;
};

int main() {
    ifstream File("in.txt");

    int ver, ar;
    File >> ver;
    File >> ar;

    vector<vector<aresta>> lista;
    lista.clear();

    int** mat = (int**) malloc(sizeof(int*) * ver);
    for (int i = 0; i < ver; i++) {
        mat[i] = (int*) malloc(sizeof(int) * ver);
        for (int j = 0; j < ver; j++) {
            mat[i][j] = 0;
        }
        vector<aresta> aux;
        aux.clear();
        lista.push_back(aux);
    }

    for (int i = 0; i < ar; i++) {
        int v1, v2, p;
        File >> v1;
        File >> v2;
        File >> p;
        mat[v1 - 1][v2 - 1] = mat[v2 - 1][v1 - 1] = p;

        aresta a1, a2;
        a1.ini = a2.fim = v1;
        a2.ini = a1.fim = v2;
        a1.peso = a2.peso = p;
        lista[v1 - 1].push_back(a1);
        lista[v2 - 1].push_back(a2);
    }

    for (int i = 0; i < ver; i++) {
        for (int j = 0; j < ver; j++) {
            cout << mat[i][j] << " ";
        }
        cout << "\n";
    }

    for (int i = 0; i < ver; i++) {
        printf("%d -> ", i + 1);
        for (int j = 0; j < lista[i].size(); j++) {
            printf("(%d, %d) ", lista[i][j].fim, lista[i][j].peso);
        }
        printf("\n");
    }

    return 0;
}