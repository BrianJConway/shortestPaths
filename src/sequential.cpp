#include <iostream>
#include <queue>
#include <cstdlib>
#include "graphFuncs.h"

using namespace std;

void bellmanFord(int source, int graph[][VERTICES], int distances[], int predecessors[]);

int main()
{
  // Initialization
  int index;
  int graph[VERTICES][VERTICES];
  int distances[VERTICES];
  int predecessors[VERTICES];

    // Initialize and generate random values for graph
    initializeGraph(graph);
    genConnectedGraph(graph, IS_DIRECTED);
    outputToFile(graph);

  // Apply bellman-ford algorithm to find shortest paths
  bellmanFord(0, graph, distances, predecessors);

  // Output results
  printResults(distances, predecessors);

  // Shutdown
  return 0;
}

void bellmanFord(int source, int graph[][VERTICES], int distances[], int predecessors[])
{
  queue<int> vertexQueue;
  int index, currentVertex, newDist;

  vertexQueue.push(source);

  for (index = 0; index < VERTICES; index++)
  {
    if(source != index)
    {
      distances[index] = INF;
    }
    else
    {
      distances[index] = 0;
    }

    predecessors[index] = -1;
  }

  while (!vertexQueue.empty())
  {
    currentVertex = vertexQueue.front();
    vertexQueue.pop();

    for (index = 0; index < VERTICES; index++)
    {
      if (graph[currentVertex][index] != INF)
      {
        newDist = distances[currentVertex] + graph[currentVertex][index];

        if (newDist < distances[index])
        {
          distances[index] = newDist;
          predecessors[index] = currentVertex;
          vertexQueue.push(index);
        }
      }
    }
  }
}