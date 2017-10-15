#include <iostream>
#include <limits>
#include <queue>

using namespace std;

const int ROWS = 6;
const int COLS = 6;

const int INF = std::numeric_limits<int>::max();

int graph[ROWS][COLS] =
{
  { INF,  10, INF, INF, INF, INF },
  { INF, INF,   8,  13,  24,  51 },
  { INF, INF, INF,  14, INF, INF },
  { INF, INF, INF, INF,   9, INF },
  { INF, INF, INF, INF, INF,  17 },
  { INF, INF, INF, INF, INF, INF },
};

void bellmanFord(int[][COLS], int source);

int main()
{

  bellmanFord(graph, 0);

  system("pause");

  return 0;
}


void bellmanFord(int[][COLS], int source)
{
  queue<int> vertexQueue;
  int distances[ROWS];
  int predecessors[ROWS];
  int index, currentVertex, newDist;

  vertexQueue.push(source);

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

  while (!vertexQueue.empty())
  {
    currentVertex = vertexQueue.front();
    vertexQueue.pop();

    for (index = 0; index < ROWS; index++)
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