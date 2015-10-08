#include <stdio.h>

#define NNODE 6
#define INF 100

int nodes[NNODE] = {'A', 'B', 'C', 'D', 'E', 'F'};
int cost[NNODE][NNODE] = {
  {0, 2, 5, 1, INF, INF},
  {2, 0, 3, 2, INF, INF},
  {5, 3, 0, 3, 1, 5},
  {1, 2, 3, 0, 1, INF},
  {INF, INF, 1, 1, 0, 2},
  {INF, INF, 5, INF, 2, 0}
};

void Init(int *set, int (*cost)[NNODE], int *neighbor, int *p,
                    int *Dcost, int root);

// *** Helpers *** //
static int IsNeighbor(int *Neighbor);
static int IsFull(int *set);
static int IsSet(int *set, int index);
static char int2sym(int node);
// *** End Helpers *** //

int GetNeighbor(int *neighbor);
int GetMinCostNode(int *set, int *Dcost, int (*cost)[NNODE]);

void InsertNodeToSet(int *set, int index);
void InsertNodeToNeighbor(int *neighbor, int index);

void PrintDijkstraShortPath(int root, int (*cost)[NNODE]);
void UpdateNeighbor(int *neighbor, int *set, int current_node,
                    int (*cost)[NNODE]);

void PrintAnswer(int *Dcost, int *p, int root);

int main() {
  for (int i = 0; i < NNODE; i++)
    PrintDijkstraShortPath(i, cost);

  return 0;
}

void PrintDijkstraShortPath(int root, int (*cost)[NNODE]) {
  int Dcost[NNODE], p[NNODE], N_dash[NNODE], neighbor[NNODE];

  Init(N_dash, cost, neighbor, p, Dcost, root);

  while (!IsFull(N_dash)) {
    int w = GetMinCostNode(N_dash, Dcost, cost);
    InsertNodeToSet(N_dash, w);
    UpdateNeighbor(neighbor, N_dash, w, cost);
    while (IsNeighbor(neighbor)) {
      int v = GetNeighbor(neighbor);
      if (Dcost[v] > Dcost[w] + cost[w][v]) {
        Dcost[v] = Dcost[w] + cost[w][v];
        p[v] = w;
      }
    }
  }
  PrintAnswer(Dcost, p, root);
}

// Initialization
void Init(int *set, int (*cost)[NNODE], int *neighbor, int *p, int *Dcost, int root) {
  for (int i = 0; i < NNODE; i++) set[i] = -1;

  set[0] = root;
  for (int i = 0; i < NNODE; i++) neighbor[i] = -1;

  for (int i = 0; i < NNODE; i++) {
    Dcost[i] = cost[root][i];
    if (cost[root][i] == INF) {
      p[i] = -1;
    } else {
      p[i] = root;
      if (root != i) InsertNodeToNeighbor(neighbor, i);
    }
  }
}

// *** Helpers *** //
static int IsNeighbor(int *neighbor) {
  if (neighbor[0] == -1) return 0;
  return 1;
}

static int IsFull(int *set) {
  for (int i = 0; i < NNODE; i++)
    if (set[i] == -1) return 0;

  return 1;
}

static int IsSet(int *set, int index) {
  for (int i = 0; i < NNODE; i++)
    if (set[i] == index) return 1;

  return 0;
}

static char int2sym(int node) { return nodes[node]; }
// *** End Helpers *** //

int GetNeighbor(int *neighbor) {
  int node = 0;
  int i = 0;
  while (!node) {
    if (neighbor[i] == -1) node = i;
    i++;
  }
  int ret = neighbor[node - 1];
  neighbor[node - 1] = -1;
  return ret;
}

int GetMinCostNode(int *set, int *Dcost, int (*cost)[NNODE]) {
  int min = INF;
  int node = INF;
  for (int i = 0; i < NNODE; i++) {
    if (min >= Dcost[i] && !IsSet(set, i) && Dcost[i] > 0) {
      node = i;
      min = Dcost[i];
    }
  }
  if (node == INF) printf("Something wrong.....\n");

  return node;
}

void InsertNodeToSet(int *set, int index) {
  for (int i = 0; i < NNODE; i++) {
    if (set[i] == -1) {
      set[i] = index;
      break;
    }
  }
}

void InsertNodeToNeighbor(int *neighbor, int index) {
  for (int i = 0; i < NNODE; i++) {
    if (neighbor[i] == -1) {
      neighbor[i] = index;
      break;
    }
  }
}

void UpdateNeighbor(int *neighbor, int *set, int current_node,
                    int (*cost)[NNODE]) {
  for (int i = 0; i < NNODE; i++)
    neighbor[i] = -1;

  for (int i = 0; i < NNODE; i++) {
    if (cost[current_node][i] != INF && cost[current_node][i] != 0 && !IsSet(set, i)) {
      InsertNodeToNeighbor(neighbor, i);
    }
  }
}

void PrintAnswer(int *Dcost, int *p, int root) {
  printf("Root Node : %c\n\t", int2sym(root));
  for (int i = 0; i < NNODE; i++)
    printf("[%c, %c, %d] ", int2sym(i), int2sym(p[i]), Dcost[i]);

  printf("\n");
}
