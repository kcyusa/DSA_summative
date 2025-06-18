#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEVICES 20
#define ID_LENGTH 5

// Device registry
char devices[MAX_DEVICES][ID_LENGTH];
int deviceCount = 0;

// Adjacency matrix
int adjMatrix[MAX_DEVICES][MAX_DEVICES];

// Find or add a device index
int getDeviceIndex(const char *id) {
    for (int i = 0; i < deviceCount; ++i) {
        if (strcmp(devices[i], id) == 0) {
            return i;
        }
    }
    // Not found — add new
    if (deviceCount < MAX_DEVICES) {
        strcpy(devices[deviceCount], id);
        return deviceCount++;
    } else {
        printf("Device limit reached!\n");
        exit(1);
    }
}

// Add edge to adjacency matrix
void addCommunication(const char *from, const char *to) {
    int fromIdx = getDeviceIndex(from);
    int toIdx = getDeviceIndex(to);
    adjMatrix[fromIdx][toIdx] = 1; // directed communication
}

// Print adjacency matrix
void printAdjacencyMatrix() {
    printf("\nAdjacency Matrix:\n     ");
    for (int i = 0; i < deviceCount; ++i)
        printf("%s ", devices[i]);
    printf("\n");

    for (int i = 0; i < deviceCount; ++i) {
        printf("%s ", devices[i]);
        for (int j = 0; j < deviceCount; ++j) {
            printf("  %d  ", adjMatrix[i][j]);
        }
        printf("\n");
    }
}

// Show direct connections
void showDirectConnections(const char *device) {
    int idx = -1;
    for (int i = 0; i < deviceCount; ++i) {
        if (strcmp(devices[i], device) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Device %s not found.\n", device);
        return;
    }

    printf("Devices directly connected to %s:\n", device);
    int found = 0;
    for (int j = 0; j < deviceCount; ++j) {
        if (adjMatrix[idx][j]) {
            printf("- %s\n", devices[j]);
            found = 1;
        }
    }

    if (!found) {
        printf("No direct connections.\n");
    }
}

int main() {
    // Define communication logs
    addCommunication("D001", "D002");
    addCommunication("D001", "D003");
    addCommunication("D002", "D004");
    addCommunication("D003", "D005");
    addCommunication("D004", "D005");
    addCommunication("D004", "D006");
    addCommunication("D005", "D007");
    addCommunication("D006", "D008");

    // Print adjacency matrix
    printAdjacencyMatrix();

    // Query loop
    char input[ID_LENGTH];
    while (1) {
        printf("\nEnter a device ID to check connections (or 'exit' to quit): ");
        scanf("%s", input);
        if (strcmp(input, "exit") == 0) break;
        showDirectConnections(input);
    }

    return 0;
}
