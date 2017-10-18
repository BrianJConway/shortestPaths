#include "graphFuncs.h"

void initializeGraph(int graph[][VERTICES])
{
  for(int rowIndex = 0; rowIndex < VERTICES; rowIndex++)
  {
    for(int colIndex = 0; colIndex < VERTICES; colIndex++)
    {
      if(rowIndex == colIndex)
      {
        graph[rowIndex][colIndex] = 0;
      }
      else
      {
        graph[rowIndex][colIndex] = INF;
      }
    } 
  }
}

void genConnectedGraph(int graph[][VERTICES], bool isDirected)
{
  // initialize function/variables
  int outerIndex, innerIndex, rowIndex, colIndex;
  int neighborVal, randomDistance;
  random_device rd;
  default_random_engine generator(rd());
  uniform_int_distribution<int> dist(MIN_DIST, MAX_DIST);
  uniform_int_distribution<int> dist2(0, 100);
  
  // Loop while graph generated is not connected
  do
  {
    // Check if graph is directed
    if(isDirected)
    {
      // Loop through each row
      for(rowIndex = 0; rowIndex < VERTICES; rowIndex++)
      {
        // Loop through each column
        for(colIndex = 0; colIndex < VERTICES; colIndex++)
        {
          // Check if edge is not self
          if(rowIndex != colIndex)
          {
            // Generate value to check if vertices are neighbors
            neighborVal = dist2(generator);

            // Check if value indicates vertices are neighbors
            if(neighborVal <= NEIGHBOR_PROB)
            {
              // Generate random distance for the edge
              randomDistance = dist(generator);

              // Set distance
              graph[rowIndex][colIndex] = randomDistance;
            } 
          }
        }
      }
    }
    // Otherwise, assume undirected graph
    else
    {
      // Loop through each row
      for(rowIndex = 0; rowIndex < VERTICES; rowIndex++)
      {
        // Loop through columns
        for(colIndex = rowIndex + 1; colIndex < VERTICES; colIndex++)
        {
          // Generate value to check if vertices are neighbors
          neighborVal = dist2(generator);

          // Check if value indicates vertices are neighbors
          if(neighborVal <= NEIGHBOR_PROB)
          {
            // Generate random distance for the edge
            randomDistance = dist(generator);

            // Set distance
            graph[rowIndex][colIndex] = randomDistance;
            graph[colIndex][rowIndex] = randomDistance;
          }
        }
      }
    }
  }
  while(!isConnected(graph));
}

bool isConnected(int graph[][VERTICES])
{
  // initialize function/variables
  stack<int> vertices;
  int vertex;
  bool nodesVisited[VERTICES];
  int index; 
  bool isConnected = true;

    // initialize nodes visited array
    for(index = 0; index < VERTICES; index++)
    {
      nodesVisited[index] = false;
    }

  // Perform a depth-first traversal of the graph

    // Push first vertex onto the stack
    vertices.push(0);

    // Loop while stack is not empty
    while(!vertices.empty())
    {
      // Pop off current vertex
      vertex = vertices.top();
      vertices.pop();

      // Check if vertex was not visited
      if(!nodesVisited[vertex])
      {
        // label vertex as discovered
        nodesVisited[vertex] = true;

        // Loop through vertices to find neighbors
        for(index = 0; index < VERTICES; index++)
        {
          // Check if vertex is a neighbor
          if(graph[vertex][index] != INF)
          {
            // Push neighbor onto stack
            vertices.push(index);
          }
        }
      }
    }
    // Check if all nodes were visited
    for(index = 0; index < VERTICES; index++)
    {
      // Check if node was not visited
      if(!nodesVisited[index])
      {
        isConnected = false;
      }
    }

  return isConnected;
}


bool outputToFile(int graph[][VERTICES])
{
  // initialize function/variables
  ofstream fout;

  // Open output file
  fout.open("graph.csv");

  // Print graph to file in csv format
  for(int rowIndex = 0; rowIndex < VERTICES; rowIndex++)
  {
    for(int colIndex = 0; colIndex < VERTICES; colIndex++)
    {
      fout << graph[rowIndex][colIndex] << ", ";
    } 

    fout << endl;
  }

  fout.close();
}

bool printResults(int distances[], int predecessors[])
{
  // initialize function/variables
  int index;
  
  cout << "Distances: ";
  for(index = 0; index < VERTICES; index++) 
  {
    cout << distances[index] << " ";
  }
  cout << endl;

  cout << "Predecessors: ";
  for(index = 0; index < VERTICES; index++) 
  {
    cout << predecessors[index] << " ";
  }
  cout << endl;
}