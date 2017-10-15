#include <mpi.h>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <queue>

using namespace std;

const int ROWS = 6;
const int COLS = 6;
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


int main(int argc, char *argv[])
{
  // Initialization
  int numTasks, rank, dest, src, requestTag = 1;
  int responseTag = 2;
  int index, currentVertex, newVertex, newDist, newPred, source;
  int distances[ROWS];
  int predecessors[ROWS];
  MPI_Status status;
  queue<int> vertexQueue;
  int counter = 0;
  int termination = -1;

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
                
        // Get vertex from any worker node
        MPI_Recv(&newVertex, 1, MPI_INT, MPI_ANY_SOURCE, responseTag, MPI_COMM_WORLD, &status);      
        src = status.MPI_SOURCE;

        // Check if master was sent a valid vertex
        if(newVertex != -1)
        {
          // Get new distance and predecessor to vertex
          MPI_Recv(&newDist, 1, MPI_INT, src, responseTag, MPI_COMM_WORLD, &status);        
          MPI_Recv(&newPred, 1, MPI_INT, src, responseTag, MPI_COMM_WORLD, &status);        
          
          // Check if new value is actually an improvement
          if(newDist < distances[newVertex])
          {
            // Update distance and predecessor matrix 
            distances[newVertex] = newDist;
            predecessors[newVertex] = newPred;

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
          }
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

              // Send master new vertex to add to queue
              MPI_Send(&index, 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD);   
              
              // Send master new predecessor to current vertex and new current optimal distance
              MPI_Send(&distances[index], 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD);   
              MPI_Send(&predecessors[index], 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD);                 
            }
          }
        }
        
        // Check if no new paths found
        if(counter == 0)
        {
          // Send master indication that no paths were found
          MPI_Send(&termination, 1, MPI_INT, MASTER, responseTag, MPI_COMM_WORLD);             
        }

        // Request new task from master
        MPI_Send(&rank, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD);
        MPI_Recv(&newVertex, 1, MPI_INT, MASTER, requestTag, MPI_COMM_WORLD, &status);             
      }
    }
}
