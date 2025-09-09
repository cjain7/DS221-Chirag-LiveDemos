#include <iostream>
#include <queue>
#include <vector>
#include <list>
#include <iomanip>
#include <chrono>
#include <random>
#include <cmath>
using namespace std;
using namespace std::chrono;

// ================== Hash Table with Linear Probing ==================
class HashTableLinear {
    vector<int> table;
    int m;
    int EMPTY = -1;

public:
    HashTableLinear(int size) : m(size) {
        table.assign(m, EMPTY);
    }

    void insert(int key) {
        int idx = key % m;
        while (table[idx] != EMPTY) {
            idx = (idx + 1) % m; // linear probing
        }
        table[idx] = key;
    }

    bool search(int key) {
        int idx = key % m;
        int start = idx;
        while (table[idx] != EMPTY) {
            if (table[idx] == key) return true;
            idx = (idx + 1) % m;
            if (idx == start) break; // full cycle
        }
        return false;
    }
};

// ================== Hash Table with Separate Chaining ==================
class HashTableChaining {
    vector<list<int>> table;
    int m;

public:
    HashTableChaining(int size) : m(size) {
        table.resize(m);
    }

    void insert(int key) {
        int idx = key % m;
        table[idx].push_back(key);
    }

    bool search(int key) {
        int idx = key % m;
        for (auto &x : table[idx]) {
            if (x == key) return true;
        }
        return false;
    }
};

// ================== Binary Search Tree (unbalanced) ==================
struct BSTNode {
    int key;
    BSTNode *left, *right;
    BSTNode(int k) : key(k), left(nullptr), right(nullptr) {}
};

class BST {
    BSTNode* root = nullptr;

    BSTNode* insertNode(BSTNode* node, int key) {
        if (!node) return new BSTNode(key);
        if (key < node->key) node->left = insertNode(node->left, key);
        else node->right = insertNode(node->right, key);
        return node;
    }

    bool searchNode(BSTNode* node, int key) {
        if (!node) return false;
        if (node->key == key) return true;
        return key < node->key ? searchNode(node->left, key) : searchNode(node->right, key);
    }

public:
    void insert(int key) {
        root = insertNode(root, key);
    }

    bool search(int key) {
        return searchNode(root, key);
    }
};

// ================== Performance Test ==================
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int N = 20000000;   // number of keys
    const int M = 30000000;   // table size (for low load factor)

    // Generate random keys
    vector<int> keys(N);
    for (int i = 0; i < N; i++) keys[i] = rand();

    // ===== Linear Probing =====
    HashTableLinear htLinear(M);

    auto start = high_resolution_clock::now();
    for (int k : keys) htLinear.insert(k);
    auto end = high_resolution_clock::now();
    cout << "Linear probing insertion time: "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    int found = 0;
    for (int k : keys) if (htLinear.search(k)) found++;
    end = high_resolution_clock::now();
    cout << "Linear probing search time: "
         << duration_cast<nanoseconds>(end - start).count() << " ns\n";

    // ===== Chaining =====
    HashTableChaining htChain(M);

    start = high_resolution_clock::now();
    for (int k : keys) htChain.insert(k);
    end = high_resolution_clock::now();
    cout << "Chaining insertion time: "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    found = 0;
    for (int k : keys) if (htChain.search(k)) found++;
    end = high_resolution_clock::now();
    cout << "Chaining search time: "
         << duration_cast<nanoseconds>(end - start).count() << " ns\n";

    // ===== Binary Search Tree =====
    BST bst;
    start = high_resolution_clock::now();
    for (int k : keys) bst.insert(k);
    end = high_resolution_clock::now();
    cout << "BST insertion: "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    found = 0;
    for (int k : keys) if (bst.search(k)) found++;
    end = high_resolution_clock::now();
    cout << "BST search: "
         << duration_cast<nanoseconds>(end - start).count() << " ns\n";

    return 0;
}

