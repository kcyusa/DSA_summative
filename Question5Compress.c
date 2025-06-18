
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

typedef struct MinHeapNode
{
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
} MinHeapNode;

typedef struct MinHeap
{
    unsigned size;
    unsigned capacity;
    MinHeapNode **array;
} MinHeap;

MinHeapNode *newNode(char data, unsigned freq)
{
    MinHeapNode *temp = (MinHeapNode *)malloc(sizeof(MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

MinHeap *createMinHeap(unsigned capacity)
{
    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode **)malloc(minHeap->capacity * sizeof(MinHeapNode *));
    return minHeap;
}

void swapMinHeapNode(MinHeapNode **a, MinHeapNode **b)
{
    MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap *minHeap, int idx)
{
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;
    if (smallest != idx)
    {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(MinHeap *minHeap)
{
    return (minHeap->size == 1);
}

MinHeapNode *extractMin(MinHeap *minHeap)
{
    MinHeapNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap *minHeap, MinHeapNode *minHeapNode)
{
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq)
    {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildMinHeap(MinHeap *minHeap)
{
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

void printCodes(MinHeapNode *root, int arr[], int top, FILE *out, char codes[256][256])
{
    if (root->left)
    {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1, out, codes);
    }
    if (root->right)
    {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1, out, codes);
    }
    if (!(root->left) && !(root->right))
    {
        if (root->data == '\n')
            fprintf(out, "\\n:%d:", top);
        else if (root->data == '\r')
            fprintf(out, "\\r:%d:", top);
        else if (root->data == ':')
            fprintf(out, "\\c:%d:", top); // escape colon
        else
            fprintf(out, "%c:%d:", root->data, top);
        for (int i = 0; i < top; ++i)
        {
            fprintf(out, "%d", arr[i]);
            codes[(unsigned char)root->data][i] = arr[i] + '0';
        }
        codes[(unsigned char)root->data][top] = '\0';
        fprintf(out, "\n");
    }
}

MinHeap *createAndBuildMinHeap(char data[], int freq[], int size)
{
    MinHeap *minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

MinHeapNode *buildHuffmanTree(char data[], int freq[], int size)
{
    MinHeapNode *left, *right, *top;
    MinHeap *minHeap = createAndBuildMinHeap(data, freq, size);
    while (!isSizeOne(minHeap))
    {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void compressFile(const char *filename)
{
    FILE *in = fopen(filename, "r");
    if (!in)
    {
        perror("File open failed");
        return;
    }

    int freq[256] = {0};
    int ch;
    while ((ch = fgetc(in)) != EOF)
        freq[ch]++;
    fseek(in, 0, SEEK_SET);

    char data[256];
    int actualFreq[256];
    int size = 0;
    for (int i = 0; i < 256; ++i)
    {
        if (freq[i])
        {
            data[size] = i;
            actualFreq[size] = freq[i];
            size++;
        }
    }

    MinHeapNode *root = buildHuffmanTree(data, actualFreq, size);
    int arr[MAX_TREE_HT], top = 0;
    FILE *out = fopen("compressed.txt", "w");
    char codes[256][256] = {{0}};
    printCodes(root, arr, top, out, codes);
    fprintf(out, "DATA\n");

    while ((ch = fgetc(in)) != EOF)
        fprintf(out, "%s", codes[ch]);

    fclose(in);
    fclose(out);

    long orig_size, comp_size;
    in = fopen(filename, "r");
    fseek(in, 0L, SEEK_END);
    orig_size = ftell(in);
    fclose(in);

    out = fopen("compressed.txt", "r");
    fseek(out, 0L, SEEK_END);
    comp_size = ftell(out);
    fclose(out);
}

int main()
{
    compressFile("patient.txt");
    return 0;
}
