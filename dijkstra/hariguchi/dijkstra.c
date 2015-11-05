#include <stdio.h>

#define NNODE 6
#define INF 100 // infinity

int nodes[NNODE] = {'A', 'B', 'C', 'D', 'E', 'F'};

int cost[NNODE][NNODE] = {
  {0, 2, 5, 1, INF, INF},
  {2, 0, 3, 2, INF, INF},
  {5, 3, 0, 3, 1, 5},
  {1, 2, 3, 0, 1, INF},
  {INF, INF, 1, 1, 0, 2},
  {INF, INF, 5, INF, 2, 0}
};

int main(void){


  for(int i = 0; i < NNODE; i++){
    PrintDijkstraShortestPath(i, cost);
  }

  return 0;
}
