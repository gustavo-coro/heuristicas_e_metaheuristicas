#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

// Compile with sanitizer: g++ activity5.cpp -o activity5 -fsanitize=address -static-libasan -g

using namespace std;

// Structure to represent a city with id, x-coordinate, and y-coordinate
struct City {
    int id;
    int x;
    int y;
};

// Structure to represent a solution with a vector of city ids and total distance
struct Solution {
    std::vector<int> cities;
    float distance;
};

// Function to generate a random solution (random permutation of cities)
vector<int> generateRandomSolution(int numCities, float **adjMatrix) {
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_int_distribution<int> dist{0, numCities};

    std::vector<int> cities(numCities);
    std::iota(begin(cities), end(cities), 1);
    std::shuffle(begin(cities), end(cities), mersenne_engine);
    return cities;
}

// Function to generate a neighbor solution by swapping two random cities
vector<int> generateRandomNeighbor(int numCities, float **adjMatrix, Solution currentSolution) {
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_int_distribution<int> dist{0, numCities - 1};

    int i = dist(mersenne_engine);
    uniform_int_distribution<int> new_dist{i, numCities - 1};
    int j = new_dist(mersenne_engine);

    std::vector<int> neighbor = currentSolution.cities;
    swap(neighbor[i], neighbor[j]);

    return neighbor;
}

// Function to calculate the total distance of a solution
float calculateTotalDistance(vector<int> solution, int numCities, float **adjMatrix) {
    float totalDistance = 0.0;
    for (int i = 0; i < numCities; i++) {
        if (i == (numCities - 1)) {
            totalDistance += adjMatrix[solution[i] - 1][solution[0] - 1];
        } else {
            totalDistance += adjMatrix[solution[i] - 1][solution[i + 1] - 1];
        }
    }
    return totalDistance;
}

// Function to calculate the acceptance probability for a worse solution in simulated annealing
bool calculateAcceptanceProbability(float temperature, float currentDistance, float neighborDistance) {
    const double EulerConstant = exp(1.0);
    random_device rnd_device;
    mt19937 mersenne_engine{rnd_device()};
    uniform_real_distribution<float> dist{0, 1};

    float r = dist(mersenne_engine);
    float exponent = (currentDistance - neighborDistance) / temperature;
    float probability = pow(EulerConstant, exponent);

    return r < probability;
}

// Main function
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error in parameter passing\n");
        return -1;
    }

    ifstream inputFile(argv[1]);

    int maxIterations = 250;
    float initialTemperature = 800;
    float coolingRate = 0.99;

    int numCities;
    inputFile >> numCities;

    vector<City> cities;

    float **adjMatrix = (float **)malloc(sizeof(float *) * numCities);
    for (int i = 0; i < numCities; i++) {
        adjMatrix[i] = (float *)malloc(sizeof(float) * numCities);
        for (int j = 0; j < numCities; j++) {
            adjMatrix[i][j] = 0.0;
        }
    }

    for (int i = 0; i < numCities; i++) {
        City city;
        inputFile >> city.id >> city.x >> city.y;
        cities.push_back(city);
    }

    for (int i = 0; i < numCities; i++) {
        int idx_first = cities[i].id - 1;
        for (int j = 0; j < numCities; j++) {
            int idx_second = cities[j].id - 1;
            int x_diff = cities[i].x - cities[j].x;
            int y_diff = cities[i].y - cities[j].y;
            adjMatrix[idx_first][idx_second] = sqrt(x_diff * x_diff + y_diff * y_diff);
        }
    }

    inputFile.close();

    srand(time(NULL));
    clock_t startTime = clock();

    vector<int> initialSolution = generateRandomSolution(numCities, adjMatrix);
    float initialDistance = calculateTotalDistance(initialSolution, numCities, adjMatrix);

    Solution currentSolution;
    currentSolution.distance = initialDistance;
    currentSolution.cities = initialSolution;

    printf("Initial Distance = %f\n", currentSolution.distance);
    printf("Initial Solution = ");
    for (int i = 0; i < numCities; i++) {
        printf("[%d] ", currentSolution.cities[i]);
    }
    printf("\n");

    Solution bestSolution = currentSolution;

    float temperature = initialTemperature;
    int iteration = 0;

    while (temperature > 1e-10) {
        while (iteration < maxIterations) {
            iteration++;
            Solution neighborSolution;
            neighborSolution.cities = generateRandomNeighbor(numCities, adjMatrix, currentSolution);
            neighborSolution.distance = calculateTotalDistance(neighborSolution.cities, numCities, adjMatrix);

            if (neighborSolution.distance < currentSolution.distance) {
                currentSolution = neighborSolution;
                if (neighborSolution.distance < bestSolution.distance) {
                    bestSolution = neighborSolution;
                }
            } else if (calculateAcceptanceProbability(temperature, currentSolution.distance, neighborSolution.distance)) {
                currentSolution = neighborSolution;
            }
        }
        printf("Best Distance so far = %f\n", bestSolution.distance);
        temperature *= coolingRate;
        iteration = 0;
    }

    startTime = clock() - startTime;
    printf("Execution Time: %f seconds.\n", ((float)startTime) / CLOCKS_PER_SEC);

    printf("Best Distance = %f\n", bestSolution.distance);
    printf("Best Solution = ");
    for (int i = 0; i < numCities; i++) {
        printf("[%d] ", bestSolution.cities[i]);
    }
    printf("\n");

    for (int i = 0; i < numCities; i++) {
        free(adjMatrix[i]);
    }
    free(adjMatrix);

    return 0;
}
