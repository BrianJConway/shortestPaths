#ifndef GRAPHFUNCS_H
#define GRAPHFUNCS_H

#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <cstdlib>
#include <random>
#include <limits>
#include <algorithm>

using namespace std;

const int VERTICES = 5;

const int MIN_DIST = 1;
const int MAX_DIST = 10;

const int NEIGHBOR_PROB = 10;

const bool IS_DIRECTED = true;

const int INF = std::numeric_limits<int>::max();

void initializeGraph(int graph[][VERTICES]);

void genConnectedGraph(int graph[][VERTICES]);

bool outputToFile(int graph[][VERTICES]);

bool isConnected(int graph[][VERTICES]);

int tarjan(int graph[][VERTICES]);

void strongConnect(
  int vertex,
  int graph[][VERTICES], 
  stack<int>& vertexStack, 
  int& numNodesVisited, 
  int& numSccs, 
  int whenVisited[], 
  int roots[], 
  bool isOnStack[] );

bool depthFirstSearch(int graph[][VERTICES]);

bool printResults(int distances[], int predecessors[]);

#endif