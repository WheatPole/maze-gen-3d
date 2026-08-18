#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <unordered_map>
#include "triplet.h"

template <typename T>
class DisjointSet
{
private:
    struct Node {
        T value;
        Node *parent;

        Node *next;
        Node *tail;

        int size;



        Node(T& _value) : value(_value) {
            parent = this;
            tail = this;
            size = 1;
        }
    };

    template <typename T1>
    struct hash {
        size_t operator()(const T1& t) const {
            return std::hash<T1>{}(t);
        }
    };

    template <typename T1>
    struct hash<triplet<T1>> {
        size_t operator()(const triplet<T1>& t) const {
            size_t h1 = std::hash<T1>{}(t.x);
            size_t h2 = std::hash<T1>{}(t.y);
            size_t h3 = std::hash<T1>{}(t.z);

            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
public:
    DisjointSet();

    std::unordered_map<T, Node*, hash<T>> nodes;

    void add(T node);
    Node* findParent(Node* node);
    T find(T value);

    void join(T a, T b);
};

template <typename T>
DisjointSet<T>::DisjointSet() {

}

template <typename T>
void DisjointSet<T>::add(T node) {
    nodes.insert_or_assign(node, new Node(node));
}

template <typename T>
typename DisjointSet<T>::Node* DisjointSet<T>::findParent(Node* node) {
    if (node->parent != node) {
        node->parent = findParent(node->parent);
    }
    return node->parent;
}

template <typename T>
T DisjointSet<T>::find(T value) {
    return findParent(nodes[value])->value;
}

template <typename T>
void DisjointSet<T>::join(T a, T b) {
    Node* rootA = findParent(nodes[a]);
    Node* rootB = findParent(nodes[b]);

    if (rootA == rootB)
        return;

    // Keep the larger group as the root
    if (rootA->size < rootB->size) {
        Node* temp = rootA;
        rootA = rootB;
        rootB = temp;
    }

    // Merge linked lists
    rootA->tail->next = rootB;
    rootA->tail = rootB->tail;

    // Merge DSU trees
    rootB->parent = rootA;
    rootA->size += rootB->size;
}

#endif // DISJOINTSET_H
