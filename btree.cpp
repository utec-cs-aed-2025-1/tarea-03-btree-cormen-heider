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
    if (!root) return;
    if (!search(key)) return;
    int t = (M + 1) / 2;

    Node<TK>** path = new Node<TK>*[1000];
    int* indexStack = new int[1000];
    int top = -1;

    Node<TK>* current = root;
    while (current != nullptr) {
        int i = 0;
        while (i < current->count && key > current->keys[i]) {
            i++;
        }

        path[++top] = current;
        indexStack[top] = i;
        if (i < current->count && key == current->keys[i]) {
            // Caso 1: La clave está en una hoja
            if (current->leaf) {
                for (int j = i; j < current->count - 1; j++) {
                    current->keys[j] = current->keys[j + 1];
                }
                current->count--;
            }
            // Caso 2: La clave está en un nodo interno
            else {
                Node<TK>* sucesor = current->children[i + 1];
                while (!sucesor->leaf) {
                    sucesor = sucesor->children[0];
                }
                TK sucesorKey = sucesor->keys[0];
                current->keys[i] = sucesorKey;
                key = sucesorKey;
                current = current->children[i + 1];
                continue;
            }
            break;
        }

        if (current->leaf) {
            break;
        }

        current = current->children[i];
    }

    // Rebalancear el árbol desde abajo hacia arriba
    while (top > 0) {
        Node<TK>* node = path[top];
        Node<TK>* parent = path[top - 1];
        int idx = indexStack[top - 1];

        if (node->count >= t - 1) {
            break;
        }

        if (idx > 0 && parent->children[idx - 1]->count >= t) {
            Node<TK>* ls = parent->children[idx - 1];
            for (int i = node->count; i > 0; i--) {
                node->keys[i] = node->keys[i - 1];
            }

            if (!node->leaf) {
                for (int i = node->count + 1; i > 0; i--) {
                    node->children[i] = node->children[i - 1];
                }
                node->children[0] = ls->children[ls->count];
            }

            node->keys[0] = parent->keys[idx - 1];
            parent->keys[idx - 1] = ls->keys[ls->count - 1];
            node->count++;
            ls->count--;
        }else if (idx < parent->count && parent->children[idx + 1]->count >= t) {
            Node<TK>* rs = parent->children[idx + 1];
            node->keys[node->count] = parent->keys[idx];
            if (!node->leaf) {
                node->children[node->count + 1] = rs->children[0];
            }

            parent->keys[idx] = rs->keys[0];

            for (int i = 0; i < rs->count - 1; i++) {
                rs->keys[i] = rs->keys[i + 1];
            }

            if (!rs->leaf) {
                for (int i = 0; i < rs->count; i++) {
                    rs->children[i] = rs->children[i + 1];
                }
            }

            node->count++;
            rs->count--;
        } else {
            if (idx < parent->count) {
                Node<TK>* rs = parent->children[idx + 1];
                node->keys[node->count] = parent->keys[idx];
                node->count++;

                for (int i = 0; i < rs->count; i++) {
                    node->keys[node->count] = rs->keys[i];
                    node->count++;
                }

                if (!node->leaf) {
                    for (int i = 0; i <= rs->count; i++) {
                        node->children[node->count - rs->count + i] = rs->children[i];
                    }
                }

                for (int i = idx; i < parent->count - 1; i++) {
                    parent->keys[i] = parent->keys[i + 1];
                }

                for (int i = idx + 1; i < parent->count; i++) {
                    parent->children[i] = parent->children[i + 1];
                }

                parent->count--;
                delete rs;
            } else {
                Node<TK>* lf = parent->children[idx - 1];
                lf->keys[lf->count] = parent->keys[idx - 1];
                lf->count++;
                for (int i = 0; i < node->count; i++) {
                    lf->keys[lf->count] = node->keys[i];
                    lf->count++;
                }

                if (!node->leaf) {
                    for (int i = 0; i <= node->count; i++) {
                        lf->children[lf->count - node->count + i] = node->children[i];
                    }
                }

                for (int i = idx - 1; i < parent->count - 1; i++) {
                    parent->keys[i] = parent->keys[i + 1];
                }

                for (int i = idx; i < parent->count; i++) {
                    parent->children[i] = parent->children[i + 1];
                }

                parent->count--;
                delete node;
            }
        }

        top--;
    }

    if (root->count == 0) {
        Node<TK>* temp = root;
        if (root->leaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
        delete temp;
    }

    delete[] path;
    delete[] indexStack;
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
    string result = "";

    if (!root) return "";

    Node<TK>** stackManual = new Node<TK>*[1000];
    int* indexStack = new int[1000];
    int top = -1;

    Node<TK>* current = root;
    int currentIndex = 0;

    while (current != nullptr || top >= 0) {
        while (current != nullptr && currentIndex <= current->count) {
            stackManual[++top] = current;
            indexStack[top] = currentIndex;

            if (!current->leaf && currentIndex < current->count) {
                current = current->children[currentIndex];
                currentIndex = 0;
            } else {
                break;
            }
        }

        if (top >= 0) {
            current = stackManual[top];
            currentIndex = indexStack[top];
            top--;

            if (currentIndex < current->count) {
                result += std::to_string(current->keys[currentIndex]) + sep;
                currentIndex++;

                if (!current->leaf && currentIndex <= current->count) {
                    current = current->children[currentIndex];
                    currentIndex = 0;
                } else {
                    current = nullptr;
                }
            } else {
                current = nullptr;
            }
        }
    }

    delete[] stackManual;
    delete[] indexStack;

    if (!result.empty() && result.length() >= sep.length()) {
        result = result.substr(0, result.length() - sep.length());
    }

    return result;
}

template <typename TK>
vector<TK> BTree<TK>::rangeSearch(TK begin, TK end) {
    // TODO: Implementar búsqueda por rango
    vector<TK> result;

    if (!root) return result;

    Node<TK>* NodePrincipal = root;

    while (!NodePrincipal->leaf) {
        int i = 0;
        while (i < NodePrincipal->count && begin > NodePrincipal->keys[i]) {
            i++;
        }
        NodePrincipal = NodePrincipal->children[i];
    }

    Node<TK>** stackManual = new Node<TK>*[1000];
    int* indexStack = new int[1000];
    int top = -1;

    Node<TK>* current = root;
    int currentIndex = 0;
    bool founded = false;

    while (current != nullptr || top >= 0) {
        while (current != nullptr && currentIndex <= current->count) {
            stackManual[++top] = current;
            indexStack[top] = currentIndex;

            if (!current->leaf && currentIndex < current->count) {
                current = current->children[currentIndex];
                currentIndex = 0;
            } else {
                break;
            }
        }

        if (top >= 0) {
            current = stackManual[top];
            currentIndex = indexStack[top];
            top--;

            if (currentIndex < current->count) {
                TK key = current->keys[currentIndex];

                if (key > end) {
                    delete[] stackManual;
                    delete[] indexStack;
                    return result;
                }

                if (key >= begin) {
                    result.push_back(key);
                    founded = true;
                }

                currentIndex++;

                if (!current->leaf && currentIndex <= current->count) {
                    current = current->children[currentIndex];
                    currentIndex = 0;
                } else {
                    current = nullptr;
                }
            } else {
                current = nullptr;
            }
        }
    }

    delete[] stackManual;
    delete[] indexStack;

    return result;
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
    if (!root) return 0;

    int n = 0;

    Node<TK>** stackManual = new Node<TK>*[1000];
    int* indexStack = new int[1000];
    int top = -1;

    Node<TK>* current = root;
    int currentIndex = 0;

    while (current != nullptr || top >= 0) {
        while (current != nullptr && currentIndex <= current->count) {
            stackManual[++top] = current;
            indexStack[top] = currentIndex;

            if (!current->leaf && currentIndex < current->count) {
                current = current->children[currentIndex];
                currentIndex = 0;
            } else {
                break;
            }
        }

        if (top >= 0) {
            current = stackManual[top];
            currentIndex = indexStack[top];
            top--;

            if (currentIndex < current->count) {
                n++;
                currentIndex++;

                if (!current->leaf && currentIndex <= current->count) {
                    current = current->children[currentIndex];
                    currentIndex = 0;
                } else {
                    current = nullptr;
                }
            } else {
                current = nullptr;
            }
        }
    }

    delete[] stackManual;
    delete[] indexStack;

    return n; // Retornar el contador de elementos
}

template <typename TK>
BTree<TK>* BTree<TK>::build_from_ordered_vector(vector<TK> elements, int M) {
    // TODO: Implementar build_from_ordered_vector
    BTree<TK>* tree = new BTree<TK>(M);

    if (elements.empty()) {
        return tree;
    }

    int maxKeys = M - 1;
    Node<TK>** leafNodes = new Node<TK>*[elements.size()];
    int leafCount = 0;

    for (size_t i = 0; i < elements.size(); ) {
        Node<TK>* leaf = new Node<TK>(M);
        leaf->leaf = true;

        int keysNode = 0;
        while (i < elements.size() && keysNode < maxKeys) {
            leaf->keys[keysNode] = elements[i];
            keysNode++;
            i++;
        }
        leaf->count = keysNode;

        leafNodes[leafCount++] = leaf;
    }

    Node<TK>** currentLevel = leafNodes;
    int currentLevelCount = leafCount;

    while (currentLevelCount > 1) {
        Node<TK>** nextLevel = new Node<TK>*[currentLevelCount];
        int nextLevelCount = 0;
        for (int i = 0; i < currentLevelCount; ) {
            Node<TK>* parent = new Node<TK>(M);
            parent->leaf = false;

            int childrenCount = 0;
            while (i < currentLevelCount && childrenCount < M) {
                parent->children[childrenCount] = currentLevel[i];

                if (childrenCount > 0) {
                    parent->keys[childrenCount - 1] = currentLevel[i]->keys[0];
                }

                childrenCount++;
                i++;
            }

            parent->count = childrenCount - 1;
            nextLevel[nextLevelCount++] = parent;
        }
        if (currentLevel != leafNodes) {
            delete[] currentLevel;
        }

        currentLevel = nextLevel;
        currentLevelCount = nextLevelCount;
    }
    tree->root = currentLevel[0];

    delete[] leafNodes;
    if (currentLevel != leafNodes) {
        delete[] currentLevel;
    }

    return tree;
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
