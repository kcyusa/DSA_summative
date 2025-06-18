#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NAME_LENGTH 100
#define LOG_FILE "unauthorized_log.txt"
#define NAME_FILE "authorized_names.txt"

// BST node
typedef struct Node {
    char name[MAX_NAME_LENGTH];
    struct Node *left;
    struct Node *right;
} Node;

// Create a new BST node
Node* createNode(char *name) {
    Node *newNode = malloc(sizeof(Node));
    strcpy(newNode->name, name);
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Insert a name into BST
Node* insert(Node *root, char *name) {
    if (!root) return createNode(name);
    if (strcmp(name, root->name) < 0)
        root->left = insert(root->left, name);
    else if (strcmp(name, root->name) > 0)
        root->right = insert(root->right, name);
    return root;
}

// Search exact match in BST
int searchExact(Node *root, char *name) {
    if (!root) return 0;
    int cmp = strcmp(name, root->name);
    if (cmp == 0) return 1;
    else if (cmp < 0) return searchExact(root->left, name);
    else return searchExact(root->right, name);
}

// Levenshtein distance (edit distance)
int editDistance(const char *s1, const char *s2) {
    int len1 = strlen(s1), len2 = strlen(s2);
    int dp[len1+1][len2+1];

    for (int i = 0; i <= len1; i++) dp[i][0] = i;
    for (int j = 0; j <= len2; j++) dp[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (s1[i-1] == s2[j-1])
                dp[i][j] = dp[i-1][j-1];
            else {
                int min = dp[i-1][j-1];
                if (dp[i][j-1] < min) min = dp[i][j-1];
                if (dp[i-1][j] < min) min = dp[i-1][j];
                dp[i][j] = 1 + min;
            }
        }
    }
    return dp[len1][len2];
}

// Find closest match using edit distance
void findClosestMatch(Node *root, const char *input, char *closest, int *minDistance) {
    if (!root) return;
    int distance = editDistance(input, root->name);
    if (distance < *minDistance) {
        *minDistance = distance;
        strcpy(closest, root->name);
    }
    findClosestMatch(root->left, input, closest, minDistance);
    findClosestMatch(root->right, input, closest, minDistance);
}

// Log unrecognized names
void logUnauthorized(char *name) {
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        fprintf(f, "%s\n", name);
        fclose(f);
    }
}

// Load names from file and build BST
Node* loadAuthorizedNames(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Error opening authorized names file");
        exit(1);
    }

    Node *root = NULL;
    char buffer[MAX_NAME_LENGTH];
    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\n")] = 0; // remove newline
        if (strlen(buffer) > 0)
            root = insert(root, buffer);
    }
    fclose(f);
    return root;
}

// Free BST memory
void freeTree(Node *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main() {
    Node *root = loadAuthorizedNames(NAME_FILE);
    char input[MAX_NAME_LENGTH];

    printf("Smart Access Control System\n");
    printf("----------------------------\n");

    while (1) {
        printf("\nEnter your name (or type 'exit' to quit): ");
        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = 0; // remove newline

        if (strcmp(input, "exit") == 0) break;

        if (searchExact(root, input)) {
            printf("✅ Access Granted: Welcome, %s!\n", input);
        } else {
            char suggestion[MAX_NAME_LENGTH] = "";
            int minDist = INT_MAX;
            findClosestMatch(root, input, suggestion, &minDist);

            if (minDist <= 2) {
                printf("Did you mean \"%s\"? Access denied for \"%s\".\n", suggestion, input);
            } else {
                printf("Unrecognized name: \"%s\". Access denied.\n", input);
                logUnauthorized(input);
            }
        }
    }

    freeTree(root);
    printf("System exiting...\n");
    return 0;
}
