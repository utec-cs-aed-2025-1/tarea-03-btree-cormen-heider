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
    // Si el árbol está vacío, crear la raíz
    if (root == nullptr) {
        root = new Node<TK>(M);
        root->keys[0] = key;
        root->count = 1;
        root->leaf = true;
        return;
    }

    // Ahora buscamos el nodo HOJA donde debe ir la clave
    Node<TK>** path = new Node<TK>*[1000];  // Camino de nodos visitados
    int* childIndex = new int[1000];         // Índice del hijo por donde bajamos
    int pathSize = 0;

    Node<TK>* current = root;
    // Bajar hasta encontrar la hoja correspondiente
    while (!current->leaf) {
        // Guardar el nodo actual en el path
        path[pathSize] = current;
        // Buscar la posición donde debería ir la clave en este nodo
        int i = 0;
        while (i < current->count && key > current->keys[i]) i++;
        // Guardar por qué hijo bajamos
        childIndex[pathSize] = i;
        pathSize++;
        // Bajar por el hijo correspondiente
        current = current->children[i];
    }

    // Ahora insertar la clave ORDENADAMENTE en el nodo hoja
    // current ahora apunta al nodo hoja donde debe insertarse
    int pos = 0;
    while (pos < current->count && key > current->keys[pos]) pos++;
    // Desplazar elementos hacia la derecha para hacer espacio
    for (int i = current->count; i > pos; i--) {
        current->keys[i] = current->keys[i - 1];
    }
    // Insertamos la nueva clave
    current->keys[pos] = key;
    current->count++;

    // Ahora hacemos el split en CASCADA
    Node<TK>* nodeToSplit = current;
    TK nuevoPadre;
    Node<TK>* nuevoHermano = nullptr;
    // Mientras haya overflow del num máximo de claves en un nodo
    while (nodeToSplit != nullptr && nodeToSplit->count == M) {
        int mid = M / 2;
        // La clave del medio SUBE al padre
        nuevoPadre = nodeToSplit->keys[mid];
        // Crear el nuevo nodo hermano derecho
        nuevoHermano = new Node<TK>(M);
        nuevoHermano->leaf = nodeToSplit->leaf;
        // Mover la mitad derecha de claves al nuevo nodo
        int j = 0;
        for (int i = mid + 1; i < nodeToSplit->count; i++) {
            nuevoHermano->keys[j] = nodeToSplit->keys[i];
            j++;
        }
        nuevoHermano->count = j;
        // Si no es hoja, también mover los punteros a hijos
        int oldCount = nodeToSplit->count;
        j = 0;
        for (int i = mid + 1; i <= oldCount; i++) {
            nuevoHermano->children[j++] = nodeToSplit->children[i];
            nodeToSplit->children[i] = nullptr;
        }
        // Actualizar el count del nodo original
        nodeToSplit->count = mid;
        // Ahora subimos la clave al padre
        if (pathSize == 0) {
            // Caso donde el nodo que se partió era la raíz -> crear una NUEVA RAÍZ
            Node<TK>* newRoot = new Node<TK>(M);
            newRoot->leaf = false;
            newRoot->keys[0] = nuevoPadre;
            newRoot->count = 1;
            newRoot->children[0] = nodeToSplit;
            newRoot->children[1] = nuevoHermano;
            root = newRoot;
            nodeToSplit = nullptr; // Terminamos el loop
        } else {
            // Caso donde hay un padre -> insertar la clave promovida en el padre
            pathSize--;
            Node<TK>* parent = path[pathSize];
            int idx = childIndex[pathSize];
            // Buscar la posición correcta en el padre (mantener orden ascendente)
            pos = 0;
            while (pos < parent->count && nuevoPadre > parent->keys[pos]) pos++;
            // Desplazar claves en el padre
            for (int i = parent->count; i > pos; i--) {
                parent->keys[i] = parent->keys[i - 1];
            }
            // Desplazar punteros a nuevos hijos
            for (int i = parent->count + 1; i > pos + 1; i--) {
                parent->children[i] = parent->children[i - 1];
            }
            // Insertar la clave promovida y el nuevo hijo derecho
            parent->keys[pos] = nuevoPadre;
            parent->children[pos + 1] = nuevoHermano;
            parent->count++;
            // El padre podría ahora tener overflow -> split en cascada hacia arriba
            nodeToSplit = parent;
        }
    }
    delete[] path;
    delete[] childIndex;
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
                rs->killSelf();
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
                node->killSelf();
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
        temp->killSelf();
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
    int height = 0;
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
    struct ClearHelper {
        static void clearNode(Node<TK>* node) {
            if (node == nullptr) {
                return;
            }
            if (!node->leaf) {
                for (int i = 0; i <= node->count; i++) {
                    clearNode(node->children[i]);
                }
            }
            node->killSelf();
            delete node;
        }
    };
    ClearHelper::clearNode(root);
    root = nullptr;
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
    // Si el árbol está vacío, cumple todas las propiedades
    if (root == nullptr) return true;
    int t = (M + 1) / 2;  // Mínimo de claves permitidas, excepto enraíz
    // Idea: usar BFS con un stack para verificar todas las propiedades
    struct StackFrame {
        Node<TK>* node;
        int level;          // Nivel del nodo en el árbol
        TK minLlaves;        // Valor mínimo permitido para claves en este nodo
        TK maxLlaves;        // Valor máximo permitido para claves en este nodo
        bool hasMinLlaves;   // bool para saber si hay límite inferior
        bool hasMaxLlaves;   // bool para saber si hay límite superior
        StackFrame(Node<TK>* n, int lvl, TK min, TK max, bool hasMin, bool hasMax): node(n), level(lvl), minLlaves(min), maxLlaves(max), hasMinLlaves(hasMin), hasMaxLlaves(hasMax) {}
    };

    stack<StackFrame> st;
    st.push(StackFrame(root, 0, TK(), TK(), false, false));
    int leafLevel = -1;  // Nivel donde encontramos la primera hoja
    while (!st.empty()) {
        StackFrame frame = st.top();
        st.pop();
        Node<TK>* current = frame.node;
        // PROPIEDAD 1: Cada nodo debe tener al menos t-1 claves (M/2 - 1)
        // PERO,hay excepción con la raíz pq puede tener menos
        if (current != root) {
            if (current->count < t - 1) {
                return false;  // Nodo con muy pocas claves
            }
        }
        // También se verifica que no exceda el máximo M-1
        if (current->count >= M) {
            return false;
        }
        // PROPIEDAD 3: Los elementos en el nodo deben estar ordenados
        for (int i = 0; i < current->count - 1; i++) {
            if (current->keys[i] >= current->keys[i + 1]) {
                return false;
            }
        }
        // PROPIEDAD 4: Verificar q todas las llaves deben estar dentro del rango permitido
        for (int i = 0; i < current->count; i++) {
            if (frame.hasMinLlaves && current->keys[i] <= frame.minLlaves) {
                return false;
            }
            if (frame.hasMaxLlaves && current->keys[i] >= frame.maxLlaves) {
                return false;
            }
        }
        // PROPIEDAD 5: Todas las hojas deben estar al mismo nivel
        if (current->leaf) {
            if (leafLevel == -1) {
                leafLevel = frame.level;  // Primera hoja encontrada
            } else if (leafLevel != frame.level) {
                return false;  // Hoja a diferente nivel
            }
        }

        // Si no es hoja, procesamos los hijos
        if (!current->leaf) {
            // PROPIEDAD: Debe tener exactamente count + 1 hijos
            // Verificar que todos los punteros necesarios no sean nullptr
            for (int i = 0; i <= current->count; i++) {
                if (current->children[i] == nullptr) {
                    return false;  // Falta un hijo requerido
                }
            }
            // Agregar hijos al stack con sus límites correspondientes
            for (int i = 0; i <= current->count; i++) {
                TK newMin, newMax;
                bool hasMin = false;
                bool hasMax = false;
                if (i == 0) {
                    // Hijo más a la izquierda -> todas sus claves < keys[0]
                    newMax = current->keys[0];
                    hasMax = true;
                    if (frame.hasMinLlaves) {
                        newMin = frame.minLlaves;
                        hasMin = true;
                    }
                } else if (i == current->count) {
                    // Hijo más a la derecha -> todas sus claves > keys[count-1]
                    newMin = current->keys[current->count - 1];
                    hasMin = true;
                    if (frame.hasMaxLlaves) {
                        newMax = frame.maxLlaves;
                        hasMax = true;
                    }
                } else {
                    // Hijo intermedio -> keys[i-1] < todas sus claves < keys[i]
                    newMin = current->keys[i - 1];
                    newMax = current->keys[i];
                    hasMin = true;
                    hasMax = true;
                }
                st.push(StackFrame(current->children[i], frame.level + 1,newMin, newMax, hasMin, hasMax));
            }
        }
    }
    return true;  // Todas las propiedades se cumplen
}

template <typename TK>
BTree<TK>::~BTree() {
    clear();
}

// Instanciación explícita para tipos comunes
template class BTree<int>;
template class BTree<float>;
template class BTree<double>;
template class BTree<string>;
