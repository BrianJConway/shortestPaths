//#include <mpi.h>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <stack>

using namespace std;

const int ROWS = 6;
const int COLS = 6;

const int MIN_DIST = 0;
const int MAX_DIST = 200;

const int NEIGHBOR_PROB = 30;

const int VERTICES = 6;
const int MASTER = 0;

const int INF = std::numeric_limits<int>::max();


// Create graph adjacency matrix
int graph[ROWS][COLS] =
{
  { INF,  10, INF, INF, INF, INF },
  { INF, INF,   8,  13,  24,  51 },
  { INF, INF, INF,  14, INF, INF },
  { INF, INF, INF, INF,   9, INF },
  { INF, INF, INF, INF, INF,  17 },
  { INF, INF, INF, INF, INF, INF },
};

void genConnectedGraph(int graph[][VERTICES], bool isDirected);

bool isConnected(int graph[][VERTICES]);

void initializeGraph(int graph[][VERTICES]);

int main(int argc, char *argv[])
{
  // Initialization
  int numTasks, rank, dest, src, requestTag = 1;
  int responseTag = 2;
  int index, currentVertex, newVertex, newDist, source;
  int distances[ROWS];
  int predecessors[ROWS];
//  MPI_Status status;
  queue<int> vertexQueue;
  int counter = 0;
  int termination = -1;
  int myGraph[VERTICES][VERTICES];

    initializeGraph(myGraph);
    genConnectedGraph(myGraph, false);

    for(int rowIndex = 0; rowIndex < VERTICES; rowIndex++)
    {
      for(int colIndex = 0; colIndex < VERTICES; colIndex++)
      {
        cout << myGraph[rowIndex][colIndex] << "\t";
      }
      cout << endl;  
    }
/*
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Check for proper number of tasks/proper number of rows
    if( numTasks < 2 )
      {
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
      }

    // Set source vertex
    source = 0;

    // Initialize distances and predecessor matrices
    for (index = 0; index < ROWS; index++)
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

  // Check if master node
  if( rank == 0 )
    {
      // Push source vertex to queue
      vertexQueue.push(source);

      // Loop while there are vertices to process
      while(!vertexQueue.empty())
      {
        // Get request message from worker node
        MPI_Recv(&src, 1, MPI_INT, MPI_ANY_SOURCE, requestTag, MPI_COMM_WORLD, &status);
        
        // Extract current vertex
        currentVertex = vertexQueue.front();
        vertexQueue.pop();

        // Send worker node its task and the distance/predecessor matrices
        MPI_Send(&currentVertex, 1, MPI_INT, src, requestTag, MPI_COMM_WORLD);
        MPI_Send(distances, ROWS, MPI_INT, src, requestTag, MPI_COMM_WORLD);
        MPI_Send(predecessors, ROWS, MPI_INT, src, requestTag, MPI_COMM_WORLD);
                
        // Get vertex count from any worker node
        MPI_Recv(&counter, 1, MPI_INT, MPI_ANY_SOURCE, responseTag, MPI_COMM_WORLD, &status);      
        src = status.MPI_SOURCE;

        // Loop through all vertices that will be sent from that worker
        for(index = 0; index < counter; index++)
        {
          // Get current vertex from worker
          MPI_Recv(&newVertex, 1, MPI_INT, src, responseTag, MPI_COMM_WORLD, &status);      
          
          // Get new distance and predecessor to vertex
          MPI_Recv(&distances[newVertex], 1, MPI_INT, src, responseTag, MPI_COMM_WORLD, &status);        
          MPI_Recv(&predecessors[newVertex], 1, MPI_INT, src, responseTag, MPI_COMM_WORLD, &status);        
          
          // Append vertex to queue for re-examining
          vertexQueue.push(newVertex);
        }
      }

      cout << "Vertex queue empty" << endl;
      
      cout << "Distances: ";
      for(index = 0; index < ROWS; index++) 
      {
        cout << distances[index] << " ";
      }
      cout << endl;

      cout << "Predecessors: ";
      for(index = 0; index < ROWS; index++) 
      {
        cout << predecessors[index] << " ";
      }
      cout << endl;

      // Loop for each worker node
      counter = 0;
      while(counter > 0)
      {        
        // Get worker node's request message
        MPI_Recv(&src, 1, MPI_INT, MPI_ANY_SOURCE, requestTag, MPI_COMM_WORLD, &status);     
                
        // Send termination message 
        MPI_Send(&termination, 1, MPI_INT, src, requestTag, MPI_COMM_WORLD);        

        counter++;
      }      
    }
  // Otherwise, assume worker node
  else if(rank < numTasks)
    {
      // Send first request for task to master node
      MPI_Send(&rank, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD);

      // Get response from master
      MPI_Recv(&newVertex, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);     
            
      // Loop while response is not termination 
      while( newVertex != termination )
      {
        // Get distance and predecessor matrices from master
        MPI_Recv(distances, ROWS, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);
        MPI_Recv(predecessors, ROWS, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);
        
        // Set count of new optimal paths found
        counter = 0;

        // Loop through each vertex to consider
        for (index = 0; index < ROWS; index++)
        {
          // Check if node is neigbor to current vertex
          if (graph[newVertex][index] != INF)
          {
            // Calculate distance to current vertex if path is taken through current vertex index
            newDist = distances[newVertex] + graph[newVertex][index];
    
            // Check if path through current vertex index is shorter than current optimal distance
            if (newDist < distances[index])
            {
              counter++;

              distances[index] = newDist;
              predecessors[index] = newVertex;

              vertexQueue.push(index);                 
            }
          }
        }
        
        // Send master number of vertices to add
        MPI_Send(&counter, 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD);   
          
        // Send master each new vertex to add
        for(index = 0; index < counter; index++)
          {
            newVertex = vertexQueue.front();
            vertexQueue.pop();
  
            // Send master new vertex to add to queue
            MPI_Send(&newVertex, 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD); 
            
            // Send master new predecessor to current vertex and new current optimal distance
            MPI_Send(&distances[newVertex], 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD);   
            MPI_Send(&predecessors[newVertex], 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD);     
          }
        

        // Request new task from master
        MPI_Send(&rank, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD);
        MPI_Recv(&newVertex, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);             
      }
    }

*/
}

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