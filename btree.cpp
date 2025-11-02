#include "btree.h"
#include <iostream>
#include <string>
#include <vector>
#include <stack>

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

// Función auxiliar para convertir int a string
string intToString(int num) {
    if (num == 0) return "0";

    bool isNegative = num < 0;
    if (isNegative) num = -num;

    string result = "";
    while (num > 0) {
        result = char('0' + (num % 10)) + result;
        num /= 10;
    }

    if (isNegative) result = "-" + result;
    return result;
}

// Función auxiliar para convertir float a string
string floatToString(float num) {
    int intPart = (int)num;
    float fracPart = num - intPart;
    if (fracPart < 0) fracPart = -fracPart;

    string result = intToString(intPart);
    result += ".";

    for (int i = 0; i < 6; i++) {
        fracPart *= 10;
        int digit = (int)fracPart;
        result += char('0' + digit);
        fracPart -= digit;
    }

    return result;
}

// Función auxiliar para convertir double a string
string doubleToString(double num) {
    long long intPart = (long long)num;
    double fracPart = num - intPart;
    if (fracPart < 0) fracPart = -fracPart;

    string result = intToString((int)intPart);
    result += ".";

    for (int i = 0; i < 6; i++) {
        fracPart *= 10;
        int digit = (int)fracPart;
        result += char('0' + digit);
        fracPart -= digit;
    }

    return result;
}

// Función template auxiliar para convertir TK a string
template <typename T>
string keyToString(const T& key);

// Especialización para int
template <>
string keyToString<int>(const int& key) {
    return intToString(key);
}

// Especialización para float
template <>
string keyToString<float>(const float& key) {
    return floatToString(key);
}

// Especialización para double
template <>
string keyToString<double>(const double& key) {
    return doubleToString(key);
}

// Especialización para string
template <>
string keyToString<string>(const string& key) {
    return key;
}

template <typename TK>
string BTree<TK>::toString(const string& sep) {
    // TODO: Implementar toString
    string result = "";
    if (!root) return "";

    // Estructura para mantener el estado en el stack
    struct StackFrame {
        Node<TK>* node;
        int keyIndex;
        int state;  // Estado: 0 = no visitado, 1 = hijo izquierdo visitado, 2 = clave procesada

        StackFrame(Node<TK>* n, int idx, int s)
            : node(n), keyIndex(idx), state(s) {}
    };

    stack<StackFrame> st;
    st.push(StackFrame(root, 0, 0));

    while (!st.empty()) {
        StackFrame frame = st.top();
        st.pop();

        Node<TK>* current = frame.node;
        int keyIndex = frame.keyIndex;
        if (keyIndex > current->count) {
            continue;
        }

        if (frame.state == 0) {
            if (!current->leaf && keyIndex <= current->count) {
                st.push(StackFrame(current, keyIndex, 1));
                st.push(StackFrame(current->children[keyIndex], 0, 0));
            } else {
                st.push(StackFrame(current, keyIndex, 1));
            }
        } else if (frame.state == 1) {
            if (keyIndex < current->count) {
                result += keyToString(current->keys[keyIndex]) + sep;
                st.push(StackFrame(current, keyIndex + 1, 0));
            }
        }
    }

    // Eliminar el último separador
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

    int count = 0;

    struct StackFrame {
        Node<TK>* node;
        int keyIndex;
        int state;

        StackFrame(Node<TK>* n, int idx, int s)
            : node(n), keyIndex(idx), state(s) {}
    };

    stack<StackFrame> st;
    st.push(StackFrame(root, 0, 0));

    while (!st.empty()) {
        StackFrame frame = st.top();
        st.pop();

        Node<TK>* current = frame.node;
        int keyIndex = frame.keyIndex;

        if (keyIndex > current->count) {
            continue;
        }

        if (frame.state == 0) {
            if (!current->leaf && keyIndex <= current->count) {
                st.push(StackFrame(current, keyIndex, 1));
                st.push(StackFrame(current->children[keyIndex], 0, 0));
            } else {
                st.push(StackFrame(current, keyIndex, 1));
            }
        } else if (frame.state == 1) {
            if (keyIndex < current->count) {
                count++;
                st.push(StackFrame(current, keyIndex + 1, 0));
            }
        }
    }

    return count;
}

template <typename TK>
BTree<TK>* BTree<TK>::build_from_ordered_vector(vector<TK> elements, int M) {
    BTree<TK>* tree = new BTree<TK>(M);

    if (elements.empty()) {
        return tree;
    }

    for (size_t i = 0; i < elements.size(); i++) {
        tree->insert(elements[i]);
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
