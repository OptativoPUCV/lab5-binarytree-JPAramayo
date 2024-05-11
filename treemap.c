#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;

struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * new = (TreeMap *)malloc(sizeof(TreeMap));
    if (new == NULL) return NULL;
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    return new;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {
    if (tree == NULL || key == NULL || value == NULL) return;
    TreeNode *new = (TreeNode *)malloc(sizeof(TreeNode));
    assert(new != NULL);
    new = createTreeNode(key, value);

    if (tree->root->pair->key == NULL) {
        tree->root = new;
        tree->current = new;
        return;
    }
    TreeNode *aux = tree->root;
    while (aux != NULL) {
        if (aux->pair->key == key) return;
        if (tree->lower_than(key, aux->pair->key) == 1) {
            if (aux->left == NULL) {
                aux->left = new;
                new->parent = aux;
                tree->current = new;
                return;
            }
            aux = aux->left;
        }
        else {
            if (aux->right == NULL) {
                aux->right = new;
                new->parent = aux;
                tree->current = new;
                return;
            }
            aux = aux->right;
        }
    }
}

TreeNode * minimum(TreeNode * x){
    if (x == NULL) return NULL;
    while (x->left != NULL) x = x->left;
    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    if (tree == NULL || node == NULL) return;
    
    if (node->left == NULL && node->right == NULL) {
        if (node != tree->root) {
            if (node->parent->left == node)
                node->parent->left = NULL;
            else node->parent->right = NULL;
        }
        else tree->root = NULL;
        free(node);
    }
    else if (node->left == NULL || node->right == NULL) {
        TreeNode *child;
        if (node->left != NULL) child = node->left;
        else child = node->right;
        
        if (node != tree->root) {
            if (node->parent->left == node)
                node->parent->left = child;
            else node->parent->right = child;
            child->parent = node->parent;
        }
        else tree->root = child;
        free(node);
    }
    else {
        TreeNode* successor = minimum(node->right);
        
        int *succKey = successor->pair->key;
        void* succValue = successor->pair->value;
        
        removeNode(tree, successor);  // Eliminamos el sucesor
        node->pair->key = succKey;
        node->pair->value = succValue;
    }
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;
    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);
}




Pair * searchTreeMap(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL || key == NULL) return NULL;
    TreeNode * aux = tree->root;
    while (aux != NULL) {
        if (is_equal(tree, key, aux->pair->key)) {
            tree->current = aux;
            return aux->pair;
        }
        if (tree->lower_than(key, aux->pair->key) == 1)
            aux = aux->left;
        else
            aux = aux->right;
        tree->current = aux;
    }
    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) {
    if (tree == NULL || tree->root == NULL || key == NULL) return NULL;
    Pair * ub = NULL;
    TreeNode * aux = tree->root;
    while (aux != NULL) {
        if (is_equal(tree, key, aux->pair->key)) return aux->pair;
        else if (tree->lower_than(key, aux->pair->key) == 1) {
            ub = aux->pair;
            aux = aux->left;
        }
        else aux = aux->right;
    }
    return ub;
}

Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL) return NULL;
    TreeNode * aux = minimum(tree->root);
    tree->current = aux;
    return aux->pair;
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL) return NULL;
    TreeNode * aux = tree->current;
    if (aux->right != NULL) {
        tree->current = minimum(aux->right);
    }
    else {
        while (aux->parent != NULL && aux == aux->parent->right) {
            aux = aux->parent;
        }
        tree->current = aux->parent;
    }
    if (tree->current != NULL) return tree->current->pair;
    return NULL;
}
