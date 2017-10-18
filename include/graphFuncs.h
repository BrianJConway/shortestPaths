#ifndef GRAPHFUNCS_H
#define GRAPHFUNCS_H

#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <cstdlib>
#include <random>
#include <limits>

using namespace std;

const int VERTICES = 6;

const int MIN_DIST = 0;
const int MAX_DIST = 1000;

const int NEIGHBOR_PROB = 10;

const bool IS_DIRECTED = true;

const int INF = std::numeric_limits<int>::max();

void initializeGraph(int graph[][VERTICES]);

void genConnectedGraph(int graph[][VERTICES], bool isDirected);

bool isConnected(int graph[][VERTICES]);

bool outputToFile(int graph[][VERTICES]);

bool printResults(int distances[], int predecessors[]);

#endif