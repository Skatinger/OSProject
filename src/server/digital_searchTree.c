#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "digital_searchTree.h"
#include "keyvalue.h"


Node* createTree(int size) {
	return newNode(size, 0);
}

void destroyTree(Node* tree) {
	freeNode(tree);
}

Node* newNode(int size, int depth) {
	Node* node = (Node*) malloc(sizeof*node);
	node->left = NULL;
	node->right= NULL;
	node->hashMap = create(size);
	node->depth = depth;
	return node;
}

void freeNode(Node* node) {
	if(node == NULL) return;
	freeNode(node->left);
	freeNode(node->right);
	destroy(node->hashMap);
	free(node);
}


int getBit(char from, int position) {
	return (number >> (position)) & ~(~0 << 1);
}

Node* find(Node* root, String key) {

	Node * current = root;
	if(getBit(key, current->depth) == 0) { current = find(current, key); }
	else { current = find(current, key); }

	return current;
}

void split(Node* node) {
	Node* left = newNode(node->hashMap->size, node->depth + 1);
	Node* right = newNode(node->hashMap->size, node->depth + 1);

	for (int i = 0; i < node->hashMap->size; i++) {

		if (node->hashMap->table[i].value == NULL) continue;
		char* key = node->hashMap->table[i].key;
		// not yet completely implemented, hash keys (if necessary...)

		if(getBit(key, node->depth) == 0) {
			set(left->hashMap, key, node->hashMap->table[i].value);
		} else if(getBit(key, node->depth) == 1) {
			set(right->hashMap, key, node->hashMap->table[i].value);
		} else {
			// some handling so as not to loose the current keyValue-pair
			continue;
		}
	}

	destroy(node->hashMap);
}

void merge(Node* parentNode) {
	parentNode->hashMap = create(M);

	for (int i = 0; i < node->hashMap->size; i++) {
		if (parentNodeode->left->hashMap->table[i].value == NULL) continue;
		char* key = parentNode->left->hashMap->table[i].key;
		set(parentNode->hashMap, key, parentNode->left->hashMap->table[i].value);
	}

	for (int i = 0; i < node->hashMap->size; i++) {
		if (parentNodeode->right->hashMap->table[i].value == NULL) continue;
		char* key = parentNode->right->hashMap->table[i].key;
		set(parentNode->hashMap, key, parentNode->right->hashMap->table[i].value);
	}

	freeNode(parentNode->left);
	freeNode(parentNode->right);
}


int set(Node* root, char* key, void* value) {

	Node* node = find(root, key);
	KVS* map = node->hashMap;

	if(loadfactor(map) > 0.8) {
		split(node);
		node = find(node, key);
		map = node->hashMap;
	}

	set(map, key, value);
	return SUCCESS;
}

void* get(Node* root, char *key) {
	Node* node = find(root, key);
	return get(node->hashMap, key);
}

void* del(Node* root, char *key) {
	Node* node = find(root, key);
	return del(node->hashMap, key);
}

int replace(Node* root, char* key, void* value){
    Node* node = find(root, *key);
    int status = replace(node->hashMap, key, value);
    return status;
}

#define M 100 // table size

int main(int argc, char const *argv[]) {
    Node* root = createTree(M);
    

    insert_node(tree, seventeen);
    insert_node(tree, three_fourths);
    insert_node(tree, one_quarter);
    insert_node(tree, threehundredsomething);

    //print_tables_tree(tree);

    printf("Now removing node at 367\n");
    remove_node(tree, 367);

    print_tables_tree(tree);

    printf("Reinsertign 367 and deleting end\n");
    insert_node(tree, create_node(367));
    remove_node(tree, MAX_ADDRESS);

    print_tables_tree(tree);
    return 0;
}
