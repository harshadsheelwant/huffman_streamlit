#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

// A Huffman tree node
struct MinHeapNode {
    unsigned char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// A Min Heap: collection of min heap nodes
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

// Function to allocate a new min heap node with given character and frequency of the character
struct MinHeapNode* newNode(unsigned char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Function to create a min heap of given capacity
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// Function to swap two min heap nodes
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Standard minHeapify function
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Function to check if size of heap is 1
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// Extract minimum value node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Insert a new node to Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Function to build min heap
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// Utility function to check if this node is a leaf
int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

// Creates a min heap of capacity equal to size and inserts all characters of data[]
struct MinHeap* createAndBuildMinHeap(unsigned char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

// The main function that builds Huffman tree
struct MinHeapNode* buildHuffmanTree(unsigned char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;

    // Create a min heap & inserts all characters of data[]
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = newNode('$', left->freq + right->freq);

        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}

// Function to generate and write compressed data using Huffman codes
void writeCompressedData(struct MinHeapNode* root, int arr[], int top, FILE *outputFile) {
    if (root->left) {
        arr[top] = 0;
        writeCompressedData(root->left, arr, top + 1, outputFile);
    }

    if (root->right) {
        arr[top] = 1;
        writeCompressedData(root->right, arr, top + 1, outputFile);
    }

    if (isLeaf(root)) {
        // Write the Huffman code for this character
        fprintf(outputFile, "%c: ", root->data);
        for (int i = 0; i < top; ++i)
            fprintf(outputFile, "%d", arr[i]);
        fprintf(outputFile, "\n");
    }
}

// Function that builds a Huffman Tree and writes compressed data to output
void compressFile(unsigned char data[], int freq[], int size, const char *outputFileName) {
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);

    int arr[MAX_TREE_HT], top = 0;
    FILE *outputFile = fopen(outputFileName, "w");

    if (outputFile == NULL) {
        printf("Unable to create output file!\n");
        return;
    }

    // Write the compressed codes to the output file
    writeCompressedData(root, arr, top, outputFile);
    fclose(outputFile);
}

// Function to read the input file, calculate frequencies, and trigger compression
void huffmanCompressFile(const char *inputFileName, const char *outputFileName) {
    unsigned char data[256];
    int freq[256] = {0};
    int size = 0;

    // Open the file to be compressed
    FILE *inputFile = fopen(inputFileName, "rb");

    if (!inputFile) {
        printf("Unable to open file %s!\n", inputFileName);
        return;
    }

    unsigned char ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        freq[ch]++;
        data[size++] = ch;
    }
    fclose(inputFile);

    // Count only distinct characters
    int uniqueSize = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0)
            uniqueSize++;
    }

    // Compress the file and write the output
    compressFile(data, freq, uniqueSize, outputFileName);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(inputFileName, "rb");
if (!inputFile) {
    printf("Unable to open file %s!\n", inputFileName);
    return 1;
} else {
    printf("Successfully opened file %s.\n", inputFileName);
}


    // Compress the input file and write the compressed output
    huffmanCompressFile(argv[1], argv[2]);

    printf("Compression completed. Output written to %s\n", argv[2]);
    return 0;
}
