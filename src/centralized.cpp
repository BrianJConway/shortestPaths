#include <mpi.h>
#include "graphFuncs.h"

using namespace std;

const int MASTER = 0;

void master(int graph[][VERTICES], int distances[], int predecessors[], int numTasks, int rank, int source);

void worker(int graph[][VERTICES], int distances[], int predecessors[], int numTasks, int rank);

int main(int argc, char *argv[])
{
  // Initialization
  int numTasks, rank, index, source;
  int distances[VERTICES];
  int predecessors[VERTICES];
  int graph[VERTICES][VERTICES];

    // Initialize and generate random values for graph
    initializeGraph(graph);
    genConnectedGraph(graph);
    outputToFile(graph);

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

    // Set source vertex for paths 
    source = 0;

    // Initialize distances and predecessor arrays
    for (index = 0; index < VERTICES; index++)
    {
      if(source != index)
      {
        distances[index] = INF;
        predecessors[index] = -1;
      }
      else
      {
        distances[index] = 0;
        predecessors[index] = 0;
      }
    }

  // Check if master node
  if( rank == 0 )
    {
      master(graph, distances, predecessors, numTasks, rank, source);
    }
  // Otherwise, assume worker node
  else if(rank < numTasks)
    {
      worker(graph, distances, predecessors, numTasks, rank);      
    }

    MPI_Finalize();
    return 0;
}


void master(int graph[][VERTICES], int distances[], int predecessors[], int numTasks, int rank, int source)
{
  //initialize function/variables 
  int dest, src, requestTag = 1;
  int responseTag = 2;
  int index, currentVertex, newVertex, newDist;
  MPI_Status status;
  queue<int> vertexQueue;
  int counter = 0;
  int termination = -1;

  vertexQueue.push(source);

  // Loop while there are vertices to process
  while(!vertexQueue.empty())
  {
    // Get request message from worker node
    MPI_Recv(&src, 1, MPI_INT, MPI_ANY_SOURCE, requestTag, MPI_COMM_WORLD, &status);
        
    // Extract current vertex
    currentVertex = vertexQueue.front();
    vertexQueue.pop();
cout << "Master popping " << currentVertex << " off of queue, sending to " << src << endl;
    
    // Send worker node its task and the distance/predecessor arrays
    MPI_Send(&currentVertex, 1, MPI_INT, src, requestTag, MPI_COMM_WORLD);
    MPI_Send(distances, VERTICES, MPI_INT, src, requestTag, MPI_COMM_WORLD);
    MPI_Send(predecessors, VERTICES, MPI_INT, src, requestTag, MPI_COMM_WORLD);

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
cout << "Master pushing " << newVertex << " onto queue, from " << src << endl;
    }

    printResults(distances, predecessors);
    cout << endl << endl;
  }

  printResults(distances, predecessors);

  // Loop for each worker node
  counter = 0;
  while(counter < numTasks - 1)
  {        
    // Get worker node's request message
    MPI_Recv(&src, 1, MPI_INT, MPI_ANY_SOURCE, requestTag, MPI_COMM_WORLD, &status);     
            
    // Send termination message 
    MPI_Send(&termination, 1, MPI_INT, src, requestTag, MPI_COMM_WORLD);       
    counter++;
  }   
}

void worker(int graph[][VERTICES], int distances[], int predecessors[], int numTasks, int rank)
{
  // Initialize function/variables
  int dest, src, requestTag = 1;
  int responseTag = 2;
  int index, currentVertex, newVertex, newDist, source;
  MPI_Status status;
  queue<int> vertexQueue;
  int counter = 0;
  int termination = -1;

  // Send first request for task to master node
  MPI_Send(&rank, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD);
  
  // Get response from master
  MPI_Recv(&newVertex, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);     
        
  // Loop while master node has not indicated termination
  while( newVertex != termination )
  {
    // Get distance and predecessor arrays from master
    MPI_Recv(distances, VERTICES, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);
    MPI_Recv(predecessors, VERTICES, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);
    
    // Reset count of new optimal paths found
    counter = 0;
  
    // Loop through each vertex to consider
    for (index = 0; index < VERTICES; index++)
    {
      // Check if node is neigbor to current vertex
      if (graph[newVertex][index] != INF)
      {
        // Calculate distance to vertex at this index if path is taken through current vertex
        newDist = distances[newVertex] + graph[newVertex][index];
      
        // Check if path through current vertex is smaller than current optimal path
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