#ifndef HEADER_FILE
#define HEADER_FILE

#include "../project.h"


/* ==== structs =====*/
typedef struct Node {
	struct Node* left;
	struct Node* right; 
    struct KVS* hashMap;
    int depth;
} Node;

/*typedef struct Tree {
    struct Node* root;
    int depth;
} Tree;*/

typedef struct Node Node;

//typedef struct Node Tree;

Tree* createTree(int size);

void destroyTree(Tree* tree);

Node* newNode(int size, int depth);

void freeNode(Node* tree);

Node* find(String key);

void split(Node* node);

void merge(Node* parentNode);

#endif


