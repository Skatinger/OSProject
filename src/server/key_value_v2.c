#include "key_value_v2.h"
#include <stdio.h>
#include <stdlib.h>

static BT* create_BT() {
  BT* tree = malloc(sizeof(BT));
  tree->nbNodes = 1;
  // root is 'in the middle'
  tree->root = create_node(MAX_ADDRESS / 2);
  return tree;
}

static Node* create_node(unsigned int hash_angle) {
  Node* node = malloc(sizeof(Node));
  node->hash_angle = hash_angle;
  node->table = malloc(SLOTS * sizeof(Kvp));
  for (int i = 0; i < SLOTS; i++) {
    node->table[i] = init_kvp();
  }
  node->left = NULL;
  node->right = NULL;

  return node;
}

static void destroy_node(Node* node) {
  free(node->table);
  free(node);
}

static void insert_node(BT* tree, Node* node) {
  Node* current = tree->root;
  Node* parent = NULL;
  while (current != NULL) {
    if (node->hash_angle > current->hash_angle) {
      parent = current;
      current = current->right;
    } else {
      parent = current;
      current = current->left;
    }
  }

  if (node->hash_angle < parent->hash_angle) {
    parent->left = node;
  } else {
    parent->right = node;
  }
}

static void remove_node(BT* tree, unsigned int hash_angle) {
  // never delete the root
  if (hash_angle == MAX_ADDRESS / 2) return;

  Node* current = tree->root;
  Node* parent = NULL;

  // finding the node to remove (and its parents)
  while (current != NULL && current->hash_angle != hash_angle) {
    if (hash_angle > current->hash_angle) {
      parent = current;
      current = current->right;
    } else {
      parent = current;
      current = current->left;
    }
  }

  // deleting the reference in the parent node
  if (hash_angle < parent->hash_angle) {
    parent->left = NULL;
  } else {
    parent->right = NULL;
  }

  if (current == NULL) return; // that means the node to delete wasn't there
  Node* left = current->left;
  Node* right = current->right;

  // reinsert the kids of the node to delete
  if (left != NULL) insert_node(tree, left);
  if (right != NULL) insert_node(tree, right);

  // and destroy
  destroy_node(current);
}

static Node* find_node_after(BT* tree, unsigned int hash_angle) {
  Node* current = tree->root;
  Node* parent = NULL:

  while (current != NULL && current->hash_angle < hash_angle) {
    current
  }
}



// debug stuff


void print_nodes(Node* node) {
  if (node == NULL) return;

  print_nodes(node->left);
  printf("Node: \n");
  printf("   - Angle: %u \n\n", node->hash_angle);
  print_nodes(node->right);
}

void print_tables_tree(BT* tree) {
  print_nodes(tree->root);
}

// KVP stuff
Kvp init_kvp() {
    Kvp *kvp = malloc(sizeof(Kvp));
    kvp->key = NULL;
    kvp->value = NULL;
    return *kvp;
}


int main(int argc, char const *argv[]) {
  BT* tree = create_BT();
  Node* end = create_node(MAX_ADDRESS);
  insert_node(tree, end);
  Node* seventeen = create_node(17);
  Node* three_fourths = create_node((3 * MAX_ADDRESS)/ 4);
  Node* one_quarter = create_node(MAX_ADDRESS/4);
  Node* threehundredsomething = create_node(367);
  insert_node(tree, seventeen);
  insert_node(tree, three_fourths);
  insert_node(tree, one_quarter);
  insert_node(tree, threehundredsomething);

  print_tables_tree(tree);

  printf("Now removing node at 367\n");
  remove_node(tree, 367);

  print_tables_tree(tree);

  printf("Reinsertign 367 and deleting end\n");
  insert_node(tree, create_node(367));
  remove_node(tree, MAX_ADDRESS);

  print_tables_tree(tree);
  return 0;
}
