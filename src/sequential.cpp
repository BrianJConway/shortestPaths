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
    genConnectedGraph(graph);
    outputToFile(graph);

  // Apply bellman-ford algorithm to find shortest paths
  bellmanFord(0, graph, distances, predecessors);

  printResults(distances, predecessors);

  return 0;
}

void bellmanFord(int source, int graph[][VERTICES], int distances[], int predecessors[])
{
  // initialize function/variables
  queue<int> vertexQueue;
  int index, currentVertex, newDist;

  // Enqueue source node to vertex queue
  vertexQueue.push(source);

  // Initialize distance and predecessor matrices
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
    // Dequeue current vertex
    currentVertex = vertexQueue.front();
    vertexQueue.pop();

    // Loop through all vertices
    for (index = 0; index < VERTICES; index++)
    {
      // Check if current vertex is a neighbor 
      if (graph[currentVertex][index] != INF)
      {
        // Calculate distance to vertex at this index if path is taken through current vertex
        newDist = distances[currentVertex] + graph[currentVertex][index];

        // Check if path through current vertex is smaller than current optimal path
        if (newDist < distances[index])
        {
          // Update arrays with more optimal path
          distances[index] = newDist;
          predecessors[index] = currentVertex;

          // Push vertex at this index onto queue
          vertexQueue.push(index);
        }
      }
    }
  }
}