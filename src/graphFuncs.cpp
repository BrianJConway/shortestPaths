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

void genConnectedGraph(int graph[][VERTICES])
{
  // initialize function/variables
  int outerIndex, innerIndex, rowIndex, colIndex;
  int neighborVal, randomDistance;
  random_device rd;
  default_random_engine generator(rd());
  uniform_int_distribution<int> dist(MIN_DIST, MAX_DIST);
  uniform_int_distribution<int> dist2(0, 100);
  
  // Loop until a connected graph is generated
  do
  {
    // Reset graph
    initializeGraph(graph);
    
    // Check if graph should be directed
    if(IS_DIRECTED)
    {
      // Loop through each vertex
      for(rowIndex = 0; rowIndex < VERTICES; rowIndex++)
      {
        // Loop through each potential neighbor vertex
        for(colIndex = 0; colIndex < VERTICES; colIndex++)
        {
          // Check if edge is not self
          if(rowIndex != colIndex)
          {
            // Generate value to check if vertices are neighbors
            neighborVal = dist2(generator);

            // Check if value indicates vertices will be neighbors
            if(neighborVal <= NEIGHBOR_PROB)
            {
              // Generate random distance for the edge
              randomDistance = dist(generator);
              graph[rowIndex][colIndex] = randomDistance;
            } 
          }
        }
      }
    }
    // Otherwise, assume undirected graph
    else
    {
      // Loop through each vertex
      for(rowIndex = 0; rowIndex < VERTICES; rowIndex++)
      {
        // Loop through each potential neighbor
        for(colIndex = rowIndex + 1; colIndex < VERTICES; colIndex++)
        {
          // Generate value to check if vertices are neighbors
          neighborVal = dist2(generator);

          // Check if value indicates vertices are neighbors
          if(neighborVal <= NEIGHBOR_PROB)
          {
            // Generate random distance for the edge
            randomDistance = dist(generator);

            // Set distance symetrically 
            graph[rowIndex][colIndex] = randomDistance;
            graph[colIndex][rowIndex] = randomDistance;
          }
        }
      }
    }
  }
  while(!isConnected(graph));
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

bool isConnected(int graph[][VERTICES])
{
  // initialize function/varialbles

  if(IS_DIRECTED)
  {
cout << "Starting tarjan" << endl;

    // Use tarjan's algorithm to check if directed graph is one SCC
    return tarjan(graph) == 1;
  }
  else
  {
    // Use depth first search to check if undirected graph is connected
    return depthFirstSearch(graph);
  }
}

int tarjan(int graph[][VERTICES])
{
  // initialize functions/variables
  int numNodesVisited = 0;
  int numSccs = 0;            // Number of strongly connected components found
  int roots[VERTICES];        // Root of the tree of each vertex's strongly connected component
  int whenVisited[VERTICES]; 
  bool isOnStack[VERTICES];
  int index;
  stack<int> vertices;

    // initialize array values
    for(index = 0; index < VERTICES; index++)
    {
      isOnStack[index] = false;
      whenVisited[index] = INF;
      roots[index] = INF;
    }

  for(index = 0; index < VERTICES; index++)
  {    
    // Check if vertex not visited yet
    if(whenVisited[index] == INF)
    {
cout << "Tarjan's, vertex " << index << endl;
      
      strongConnect(index, graph, vertices, numNodesVisited, numSccs, whenVisited, roots, isOnStack);
    }
  }
  cout << "Roots: ";
  for(index = 0; index < VERTICES; index++)
  {
    cout << roots[index] << " ";
  }
  cout << endl;
  cout << "When Visited: ";
  for(index = 0; index < VERTICES; index++)
  {
    cout << whenVisited[index] << " ";
  }
  cout << endl;
cout << numSccs << endl;

  return numSccs;
}

// NOTE: BUG WHERE ROOTS ARE NOT SET PROPERLY. IF ROOTS ARE NOT ALL ZERO, IT'S BECAUSE
// NON-ZERO ROOT GUY IS NOT THE FIRST NEIGHBOR OF A VERTEX, ONLY DFS TRAVERSALS
// THAT GO IN A STRAIGHT LINE HAVE ALL ZEROS, MUST FIX
// THOUGH IT SEEMS CONNECTED GRAPHS ARE BEING GENERATED OVERALL
void strongConnect(
  int vertex,
  int graph[][VERTICES], 
  stack<int>& vertexStack, 
  int& numNodesVisited, 
  int& numSccs, 
  int whenVisited[], 
  int roots[], 
  bool isOnStack[] )
{
  // initialize function/variables
  int index;
  whenVisited[vertex] = numNodesVisited;
  roots[vertex] = numNodesVisited;
  numNodesVisited++;
  
cout << "StrongConnect vertex " << vertex << endl;
cout << "Num SCC: " << numSccs << endl;

  vertexStack.push(vertex);
  isOnStack[vertex] = true;

  // Loop through all neighbors of the vertex
  for(index = 0; index < VERTICES; index++)
  {
    if(graph[vertex][index] != INF && index != vertex)
    {
      // Check if the neighbor hasn't been visited
      if(whenVisited[index] == INF)
      {
cout << "adding neighbor of " << vertex << " to stack: " << index << endl;
        
        strongConnect(index, graph, vertexStack, numNodesVisited, numSccs, whenVisited, roots, isOnStack);
        roots[vertex] = min(roots[vertex], roots[index]);
cout << "Setting root of " << vertex << " to " << roots[vertex] << endl;
      }
      // Otherwise, check if the current neighbor is to be a member of the current component
      else if(isOnStack[index])
      {
        roots[vertex] = min(roots[vertex], whenVisited[index]);        
cout << "Setting root of " << vertex << " to " << roots[vertex] << endl;        
      }
    }
  }
  // Check if vertex is the root of it's component
  if(whenVisited[vertex] == roots[vertex])
  {
cout <<"Vertex " << vertex << " incrementing SCC count" << endl;
    numSccs++;

    // Remove all elements from stack that are part of the current SCC
    do
    {      
      index = vertexStack.top();
      vertexStack.pop();
      isOnStack[index] = false;

cout << "Pop vertex " << index << " off stack" << endl;
      
      // NOTE: Here is where we could add the current index to the SCC if we were recording them
    }    
    while(vertex != index);
  }
}

bool depthFirstSearch(int graph[][VERTICES])
{
  // initialize function/variables
  stack<int> vertices;
  int vertex, index;
  bool nodesVisited[VERTICES];
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
        nodesVisited[vertex] = true;

        // Loop through vertices to find neighbors
        for(index = 0; index < VERTICES; index++)
        {
          // Check if vertex is a neighbor
          if(graph[vertex][index] != INF && vertex != index)
          {
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