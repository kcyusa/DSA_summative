#include <stdio.h>
#include <string.h>
#include <limits.h>

#define MAX_NODES 7
#define INF 9999

const char *locations[MAX_NODES] = {
    "Dispatch Center",  // 0
    "Sector A",          // 1
    "Sector B",          // 2
    "Sector D",          // 3
    "Emergency Site",    // 4
    "Junction C",        // 5
    "Sector E"           // 6
};

int graph[MAX_NODES][MAX_NODES] = {
    {0, 10,  0, 30, 0, 0, 0},  // Dispatch Center
    {10, 0, 10,  0, 0, 0, 0},  // Sector A
    {0, 10, 0,  0, 15, 3, 0},  // Sector B
    {30, 0,  0, 0, 5, 0, 0},   // Sector D
    {0, 0, 15, 5, 0, 0, 4},    // Emergency Site
    {0, 0, 3,  0, 0, 0, 6},    // Junction C
    {0, 0, 0,  0, 4, 6, 0}     // Sector E
};

// Get index from location name
int getLocationIndex(const char *name) {
    for (int i = 0; i < MAX_NODES; i++) {
        if (strcmp(locations[i], name) == 0)
            return i;
    }
    return -1;
}

void dijkstra(int start, int end) {
    int dist[MAX_NODES], visited[MAX_NODES], prev[MAX_NODES];

    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF;
        visited[i] = 0;
        prev[i] = -1;
    }

    dist[start] = 0;

    for (int i = 0; i < MAX_NODES - 1; i++) {
        int minDist = INF, u = -1;

        for (int j = 0; j < MAX_NODES; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1)
            break;

        visited[u] = 1;

        for (int v = 0; v < MAX_NODES; v++) {
            if (graph[u][v] && !visited[v] && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                prev[v] = u;
            }
        }
    }

    if (dist[end] == INF) {
        printf("No path found.\n");
        return;
    }

    // Trace path
    int path[MAX_NODES];
    int count = 0;
    for (int at = end; at != -1; at = prev[at])
        path[count++] = at;

    printf("\nOptimal Path:\n");
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", locations[path[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\nTotal Travel Time: %d minutes\n", dist[end]);
}

int main() {
    char startName[50], endName[50];

    printf("Enter start location (e.g., Dispatch Center): ");
    fgets(startName, sizeof(startName), stdin);
    startName[strcspn(startName, "\n")] = 0;

    printf("Enter destination (e.g., Emergency Site): ");
    fgets(endName, sizeof(endName), stdin);
    endName[strcspn(endName, "\n")] = 0;

    int startIndex = getLocationIndex(startName);
    int endIndex = getLocationIndex(endName);

    if (startIndex == -1 || endIndex == -1) {
        printf("Invalid location name entered.\n");
        return 1;
    }

    dijkstra(startIndex, endIndex);
    return 0;
}
