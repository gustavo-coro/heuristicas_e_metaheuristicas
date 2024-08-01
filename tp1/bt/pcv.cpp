#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

struct City {
    int id;
    int x;
    int y;
};

struct Solution {
    std::vector<int> cities;
    float distance;
};

// Generates a random solution by shuffling the cities
vector<int> generate_random_solution(int num_cities, float **distance_matrix) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<int> dist{0, num_cities};

    std::vector<int> cities(num_cities);
    std::iota(std::begin(cities), std::end(cities), 1);
    std::shuffle(std::begin(cities), std::end(cities), mersenne_engine);
    return cities;
}

// Generates a random neighbor by swapping two cities
vector<int> generate_random_neighbor(int num_cities, float **distance_matrix,
                                     Solution current_solution) {
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<int> dist{0, num_cities - 1};

    int i = dist(mersenne_engine);
    std::uniform_int_distribution<int> new_dist{i, num_cities - 1};
    int j = new_dist(mersenne_engine);
    std::vector<int> neighbor = current_solution.cities;

    int temp = neighbor[i];
    neighbor[i] = neighbor[j];
    neighbor[j] = temp;

    return neighbor;
}

// Calculates the total distance of a solution
float calculate_total_distance(vector<int> solution, int num_cities,
                               float **distance_matrix) {
    float total_distance = 0.0;
    for (int i = 0; i < num_cities; i++) {
        if (i == (num_cities - 1)) {
            total_distance += distance_matrix[solution[i] - 1][solution[0] - 1];
            continue;
        }
        total_distance += distance_matrix[solution[i] - 1][solution[i + 1] - 1];
    }
    return total_distance;
}

// Generates all possible neighbors of a given solution
vector<vector<Solution>> generate_all_neighbors(int num_cities,
                                                float **distance_matrix,
                                                Solution current_solution) {
    vector<vector<Solution>> neighborhood(num_cities,
                                          vector<Solution>(num_cities));
    for (int i = 0; i < num_cities; i++) {
        for (int j = i; j < num_cities; j++) {
            vector<int> neighbor = current_solution.cities;
            int temp = neighbor[i];
            neighbor[i] = neighbor[j];
            neighbor[j] = temp;
            float d =
                calculate_total_distance(neighbor, num_cities, distance_matrix);
            Solution s;
            s.cities = neighbor;
            s.distance = d;
            neighborhood[i][j] = s;
        }
    }
    return neighborhood;
}

// Selects a new solution based on the tabu list and current best solution
Solution select_new_solution(Solution current_solution, Solution best_solution,
                             vector<vector<int>> &tabu_list, int tabu_size,
                             int penalty, int num_cities,
                             float **distance_matrix) {
    vector<vector<Solution>> neighborhood =
        generate_all_neighbors(num_cities, distance_matrix, current_solution);
    Solution new_solution;
    new_solution.distance = std::numeric_limits<float>::max();
    int i, j, n_i, n_j;

    // Remove oldest element from the tabu list if it's full
    if (tabu_size == num_cities) {
        int min_i = 0, min_j = 0;
        for (i = 0; i < num_cities; i++) {
            for (j = i; j < num_cities; j++) {
                if (tabu_list[min_i][min_j] == 0) {
                    break;
                }
                if (tabu_list[i][j] < tabu_list[min_i][min_j]) {
                    min_i = i;
                    min_j = j;
                }
            }
        }
        tabu_list[min_i][min_j] = 0;
    }

    // Select a new solution not in the tabu list or better than the best
    for (i = 0; i < num_cities; i++) {
        for (j = i; j < num_cities; j++) {
            // Check if the neighbor is a candidate for new solution
            if ((tabu_list[i][j] == 0) ||
                (neighborhood[i][j].distance < best_solution.distance)) {
                if (neighborhood[i][j].distance < new_solution.distance) {
                    new_solution = neighborhood[i][j];
                    n_i = i;
                    n_j = j;
                }
            }
        }
    }

    // Update the tabu list
    for (i = 0; i < num_cities; i++) {
        for (j = i; j < num_cities; j++) {
            if (tabu_list[i][j] != 0) {
                tabu_list[i][j]--;
            }
        }
    }
    tabu_list[n_i][n_j] = tabu_size;
    return new_solution;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: Incorrect number of parameters\n");
        return -1;
    }

    ifstream file(argv[1]);
    int num_cities;
    file >> num_cities;

    vector<City> cities;
    float **distance_matrix = (float **)malloc(sizeof(float *) * num_cities);
    for (int i = 0; i < num_cities; i++) {
        distance_matrix[i] = (float *)malloc(sizeof(float) * num_cities);
        for (int j = 0; j < num_cities; j++) {
            distance_matrix[i][j] = 0.0;
        }
    }

    // Read city data
    for (int i = 0; i < num_cities; i++) {
        City c;
        file >> c.id >> c.x >> c.y;
        cities.push_back(c);
    }

    // Fill distance matrix
    for (int i = 0; i < num_cities; i++) {
        int idx_f = cities[i].id - 1;
        for (int j = 0; j < num_cities; j++) {
            int idx_s = cities[j].id - 1;
            int xd = cities[i].x - cities[j].x;
            int yd = cities[i].y - cities[j].y;
            distance_matrix[idx_f][idx_s] = sqrt(xd * xd + yd * yd);
        }
    }

    file.close();

    srand(time(NULL));
    clock_t t;
    t = clock();

    vector<int> solution =
        generate_random_solution(num_cities, distance_matrix);
    float distance =
        calculate_total_distance(solution, num_cities, distance_matrix);
    Solution current_solution;
    current_solution.distance = distance;
    current_solution.cities = solution;

    printf("Distance = %f\n", current_solution.distance);
    printf("Solution = ");
    for (int i = 0; i < num_cities; i++) {
        printf("[%d] ", current_solution.cities[i]);
    }
    printf("\n");

    int penalty = 50;
    Solution best_solution = current_solution;
    vector<vector<int>> tabu_list(num_cities, vector<int>(num_cities, 0));
    int iteration = 0, best_change = 0, tabu_size = 2;

    while (best_change < 100) {
        printf("Distance = %f\n", best_solution.distance);
        current_solution = select_new_solution(current_solution, best_solution,
                                               tabu_list, tabu_size, penalty,
                                               num_cities, distance_matrix);
        if (current_solution.distance < best_solution.distance) {
            best_solution = current_solution;
            best_change = 0;
        } else {
            best_change += 1;
        }
        iteration += 1;
    }

    t = clock() - t;

    printf("Execution Time: %f seconds.\n", ((float)t) / CLOCKS_PER_SEC);
    printf("Distance = %f\n", best_solution.distance);
    printf("Solution = ");
    for (int i = 0; i < num_cities; i++) {
        printf("[%d] ", best_solution.cities[i]);
    }
    printf("\n");

    for (int i = 0; i < num_cities; i++) {
        free(distance_matrix[i]);
    }
    free(distance_matrix);

    return 0;
}
