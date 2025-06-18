
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char ch;
    struct Node *left, *right;
} Node;

Node *newNode()
{
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->ch = 0;
    temp->left = temp->right = NULL;
    return temp;
}

void insertCode(Node *root, const char *code, char ch)
{
    while (*code)
    {
        if (*code == '0')
        {
            if (!root->left)
                root->left = newNode();
            root = root->left;
        }
        else
        {
            if (!root->right)
                root->right = newNode();
            root = root->right;
        }
        code++;
    }
    root->ch = ch;
}

void decompressFile(const char *filename)
{
    FILE *in = fopen(filename, "r");
    if (!in)
    {
        perror("File open failed");
        return;
    }

    Node *root = newNode();
    char line[512], *data_section;
    while (fgets(line, sizeof(line), in))
    {
        if (strcmp(line, "DATA\n") == 0)
            break;

        char ch;
        char label[10];
        int len;
        char bits[256];

        if (sscanf(line, "%9[^:]:%d:%255s", label, &len, bits) == 3)
        {
            if (strcmp(label, "\\n") == 0)
                ch = '\n';
            else if (strcmp(label, "\\r") == 0)
                ch = '\r';
            else if (strcmp(label, "\\c") == 0)
                ch = ':'; // unescape colon
            else
                ch = label[0];

            bits[len] = '\0';
            insertCode(root, bits, ch);
        }
        else
        {
            fprintf(stderr, "Warning: failed to parse line: %s", line);
        }
    }

    FILE *out = fopen("decompressed.txt", "w");
    Node *current = root;
    char bit;
    while ((bit = fgetc(in)) != EOF)
    {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right)
        {
            fputc(current->ch, out);
            current = root;
        }
    }

    fclose(in);
    fclose(out);
    printf("Decompression completed. Output written to 'decompressed.txt'\n");
}

int main()
{
    decompressFile("compressed.txt");
    return 0;
}
