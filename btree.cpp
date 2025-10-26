#include "btree.h"
#include <iostream>
#include <vector>

using namespace std;

template <typename TK>
bool BTree<TK>::search(TK key) {
    if (root == nullptr) {
        return false;
    }
    
    Node<TK>* current = root;
    
    while (current != nullptr) {
        int i = 0;
        
        // Buscar en el nodo actual hasta encontrar una clave mayor o igual
        while (i < current->count && key > current->keys[i]) {
            i++;
        }
        
        // Si encontramos la clave exacta en este nodo
        if (i < current->count && key == current->keys[i]) {
            return true;
        }
        
        // Si es un nodo hoja y no encontramos la clave, no existe
        if (current->leaf) {
            return false;
        }
        
        // Si no es hoja, bajar por el hijo correspondiente
        current = current->children[i];
    }
    
    return false;
}

template <typename TK>
void BTree<TK>::insert(TK key) {
    // TODO: Implementar inserción
}

template <typename TK>
void BTree<TK>::remove(TK key) {
    // TODO: Implementar eliminación
}

template <typename TK>
int BTree<TK>::height() {
    if (root == nullptr) {
        return 0; // Árbol vacío tiene altura 0
    }
    int height = 1;
    Node<TK>* current = root;
    
    while (!current->leaf) {
        height++;
        // Ir por el primer hijo para calcular altura mínima
        current = current->children[0];
    }
    
    return height;
}

template <typename TK>
string BTree<TK>::toString(const string& sep) {
    // TODO: Implementar toString
    return "";
}

template <typename TK>
vector<TK> BTree<TK>::rangeSearch(TK begin, TK end) {
    // TODO: Implementar búsqueda por rango
    return vector<TK>();
}

template <typename TK>
TK BTree<TK>::minKey() {
    if (root == nullptr) {
        return TK();
    }
    
    Node<TK>* current = root;
    
    // Ir siempre por el hijo más a la izquierda hasta llegar a una hoja
    while (!current->leaf) {
        current = current->children[0]; // Siempre el primer hijo
    }
    
    // En la hoja, la primera clave es la mínima
    return current->keys[0];
}

template <typename TK>
TK BTree<TK>::maxKey() {
    if (root == nullptr) {
        return TK();
    }
    
    Node<TK>* current = root;
    
    // Ir siempre por el hijo más a la derecha hasta llegar a una hoja
    while (!current->leaf) {
        current = current->children[current->count]; // Siempre el último hijo
    }
    
    // En la hoja, la última clave es la máxima
    return current->keys[current->count - 1];
}

template <typename TK>
void BTree<TK>::clear() {
    // TODO: Implementar clear
}

template <typename TK>
int BTree<TK>::size() {
    return n; // Retornar el contador de elementos
}

template <typename TK>
BTree<TK>* BTree<TK>::build_from_ordered_vector(vector<TK> elements, int M) {
    // TODO: Implementar build_from_ordered_vector
    return new BTree<TK>(M);
}

template <typename TK>
bool BTree<TK>::check_properties() {
    // TODO: Implementar check_properties
    return true;
}

template <typename TK>
BTree<TK>::~BTree() {
    // TODO: Implementar destructor
}

// Instanciación explícita para tipos comunes
template class BTree<int>;
template class BTree<float>;
template class BTree<double>;
template class BTree<string>;
