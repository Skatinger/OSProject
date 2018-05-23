#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "digital_searchTree.h"
#include "keyvalue.h"


Tree* createTree(int size) {
	Tree* tree = (Tree*) malloc(sizeof*tree);
	tree->root = newNode(size);
	tree->depth = 0;
}

void destroyTree(Tree* tree) {
	freeNode(tree->root);
	free(tree);
}

Node* newNode(int size) {
	Node* node = (Node*) malloc(sizeof*node);
	node->left = NULL;
	node->right= NULL;
	node->hashMap = create(size);
	return node;
}

void freeNode(Node* node) {
	if(node == NULL) return;
	freeNode(node->left);
	freeNode(node->right);
	destroy(node->hashMap);
	free(node);
}

// not yet implemented
KVS* find(String key);

void split(Node* node) {
	Node* left = newNode(node->hashMap->size);
	Node* right = newNode(node->hashMap->size);
	
	for (int i = 0; i < node->hashMap->size; i++) {
		
		if (node->hashMap->table[i].value == NULL) continue;
		char* key = node->hashMap->table[i].key;
		// not yet completely implemented
	}
}

void merge(Node* parentNode) {
	//not yet implemented	
}
