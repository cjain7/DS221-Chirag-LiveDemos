#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

// ============================================================
// Configuration
// ============================================================

constexpr uint64_t BLOCK_SIZE = 4096;
constexpr uint64_t INVALID_BLOCK = UINT64_MAX;

// We use a simple fixed-size record.
struct Record {
  int64_t key;
  int64_t value;
};

// ============================================================
// Global statistics
// ============================================================

struct Statistics {
  uint64_t block_reads = 0;
  uint64_t comparisons = 0;

  void reset() {
    block_reads = 0;
    comparisons = 0;
  }
};

Statistics stats;


// ============================================================
// Disk abstraction
// ============================================================

class Disk {
  private:
    fstream file;

  public:
    Disk(const string& filename) {
      file.open(filename,
          ios::in | ios::out | ios::binary | ios::trunc);

      if (!file) {
        cerr << "Could not open file: " << filename << endl;
        exit(1);
      }
    }

    ~Disk() {
      file.close();
    }

    void write_block(uint64_t block_id, const char* data) {
      file.seekp(block_id * BLOCK_SIZE);
      file.write(data, BLOCK_SIZE);
      file.flush();
    }

    void read_block(uint64_t block_id, char* data) {
      ++stats.block_reads;

      file.seekg(block_id * BLOCK_SIZE);
      file.read(data, BLOCK_SIZE);

      if (!file) {
        cerr << "Error reading block " << block_id << endl;
        exit(1);
      }
    }
};


// ============================================================
// Disk-backed Binary Search Tree
// ============================================================

struct BSTNode {
  int64_t key;
  uint64_t left;
  uint64_t right;
};

// Number of BST nodes per block.
// We deliberately put ONE node in each block to make
// the cost of a tree traversal obvious.
class DiskBST {
  private:
    Disk& disk;
    uint64_t root = INVALID_BLOCK;
    uint64_t next_block = 0;

    void write_node(uint64_t block_id, const BSTNode& node) {
      char buffer[BLOCK_SIZE] = {};

      memcpy(buffer, &node, sizeof(node));

      disk.write_block(block_id, buffer);
    }

    BSTNode read_node(uint64_t block_id) {
      char buffer[BLOCK_SIZE];

      disk.read_block(block_id, buffer);

      BSTNode node;
      memcpy(&node, buffer, sizeof(node));

      return node;
    }

  public:
    DiskBST(Disk& d) : disk(d) {}

    uint64_t create_node(int64_t key) {
      BSTNode node;

      node.key = key;
      node.left = INVALID_BLOCK;
      node.right = INVALID_BLOCK;

      uint64_t block_id = next_block++;

      write_node(block_id, node);

      return block_id;
    }

    void insert(int64_t key) {
      if (root == INVALID_BLOCK) {
        root = create_node(key);
        return;
      }

      uint64_t current = root;

      while (true) {
        BSTNode node = read_node(current);

        ++stats.comparisons;

        if (key < node.key) {

          if (node.left == INVALID_BLOCK) {
            uint64_t child = create_node(key);

            node.left = child;
            write_node(current, node);

            return;
          }

          current = node.left;
        }
        else if (key > node.key) {

          if (node.right == INVALID_BLOCK) {
            uint64_t child = create_node(key);

            node.right = child;
            write_node(current, node);

            return;
          }

          current = node.right;
        }
        else {
          return;
        }
      }
    }

    bool search(int64_t key) {
      uint64_t current = root;

      while (current != INVALID_BLOCK) {

        BSTNode node = read_node(current);

        ++stats.comparisons;

        if (key == node.key)
          return true;

        if (key < node.key)
          current = node.left;
        else
          current = node.right;
      }

      return false;
    }
};


// ============================================================
// Disk-backed B-Tree
// ============================================================
//
// Each B-tree node occupies exactly ONE disk block.
//
// With 4 KiB blocks and 8-byte keys/pointers, we can store
// roughly hundreds of keys/children in one node.
//
// To keep the implementation simple, we use a fixed degree.
//
// ============================================================

constexpr int B = 100;

// We will use "Order 2B" tree
// Maximum keys = 2B - 1
// Maximum children = 2B
// Minimum degree of an internal node = B

struct BTreeNode {
  bool leaf;
  int n;

  int64_t keys[2 * B - 1];
  uint64_t children[2 * B];
};


class DiskBTree {
  private:
    Disk& disk;

    uint64_t root;
    uint64_t next_block = 0;

    void write_node(uint64_t block_id,
        const BTreeNode& node) {

      char buffer[BLOCK_SIZE] = {};

      memcpy(buffer, &node, sizeof(node));

      disk.write_block(block_id, buffer);
    }

    BTreeNode read_node(uint64_t block_id) {

      char buffer[BLOCK_SIZE];

      disk.read_block(block_id, buffer);

      BTreeNode node;

      memcpy(&node, buffer, sizeof(node));

      return node;
    }

    uint64_t create_node(bool leaf) {

      BTreeNode node{};

      node.leaf = leaf;
      node.n = 0;

      for (int i = 0; i < 2 * B; ++i)
        node.children[i] = INVALID_BLOCK;

      uint64_t block_id = next_block++;

      write_node(block_id, node);

      return block_id;
    }


    // Split child y of x at position i.
    void split_child(uint64_t x_block,
        int i,
        uint64_t y_block) {

      BTreeNode x = read_node(x_block);
      BTreeNode y = read_node(y_block);

      uint64_t z_block = create_node(y.leaf);

      BTreeNode z = read_node(z_block);

      z.n = B - 1;

      // Move upper half of y to z.
      for (int j = 0; j < B - 1; ++j)
        z.keys[j] = y.keys[j + B];

      if (!y.leaf) {
        for (int j = 0; j < B; ++j)
          z.children[j] = y.children[j + B];
      }

      y.n = B - 1;

      // Move x's children.
      for (int j = x.n; j >= i + 1; --j)
        x.children[j + 1] = x.children[j];

      x.children[i + 1] = z_block;

      // Move x's keys.
      for (int j = x.n - 1; j >= i; --j)
        x.keys[j + 1] = x.keys[j];

      // Middle key moves to parent.
      x.keys[i] = y.keys[B - 1];

      ++x.n;

      write_node(y_block, y);
      write_node(z_block, z);
      write_node(x_block, x);
    }


    void insert_non_full(uint64_t block,
        int64_t key) {

      BTreeNode node = read_node(block);

      int i = node.n - 1;

      if (node.leaf) {

        while (i >= 0) {

          ++stats.comparisons;

          if (key < node.keys[i]) {
            node.keys[i + 1] = node.keys[i];
            --i;
          }
          else {
            break;
          }
        }

        node.keys[i + 1] = key;
        ++node.n;

        write_node(block, node);

      }
      else {

        while (i >= 0) {

          ++stats.comparisons;

          if (key < node.keys[i])
            --i;
          else
            break;
        }

        ++i;

        BTreeNode child = read_node(node.children[i]);

        if (child.n == 2 * B - 1) {

          split_child(block,
              i,
              node.children[i]);

          node = read_node(block);

          ++stats.comparisons;

          if (key > node.keys[i])
            ++i;
        }

        insert_non_full(node.children[i], key);
      }
    }


  public:

    DiskBTree(Disk& d)
      : disk(d) {

        root = create_node(true);
      }


    void insert(int64_t key) {

      BTreeNode r = read_node(root);

      if (r.n == 2 * B - 1) {

        uint64_t new_root =
          create_node(false);

        BTreeNode root_node =
          read_node(new_root);

        root_node.children[0] = root;
        root_node.n = 0;

        write_node(new_root,
            root_node);

        uint64_t old_root = root;

        root = new_root;

        split_child(new_root,
            0,
            old_root);

        insert_non_full(root,
            key);
      }
      else {
        insert_non_full(root,
            key);
      }
    }


    bool search(int64_t key) {

      uint64_t current = root;

      while (true) {

        BTreeNode node =
          read_node(current);

        int i = 0;

        // Search within the node.
        while (i < node.n) {

          ++stats.comparisons;

          if (key == node.keys[i])
            return true;

          if (key < node.keys[i])
            break;

          ++i;
        }

        if (node.leaf)
          return false;

        current = node.children[i];
      }
    }
};


// ============================================================
// Main experiment
// ============================================================

int main() {

  constexpr int N = 100000; //number of keys
  constexpr int NUM_SEARCHES = 1000;  //number of searches for performance evaluation

  cout << "============================================\n";
  cout << "External-Memory BST vs B-Tree\n";
  cout << "============================================\n\n";

  cout << "Number of records                                : " << N << '\n';
  cout << "Block size                                       : " << BLOCK_SIZE << " bytes\n";
  cout << "Minimum degree for any internal node in B-tree   : " << B << '\n';
  cout << "Search operations                                : " << NUM_SEARCHES << "\n\n";


  // --------------------------------------------------------
  // Create disk-backed BST
  // --------------------------------------------------------

  cout << "Building disk-backed binary search tree...\n";

  Disk bst_disk("bst.dat");

  DiskBST bst(bst_disk);

  // Insert keys in random order so that the BST is
  // reasonably balanced rather than completely skewed.

  vector<int64_t> keys(N);

  for (int i = 0; i < N; ++i)
    keys[i] = i;

  mt19937_64 rng(12345);

  shuffle(keys.begin(),
      keys.end(),
      rng);

  stats.reset();

  for (int64_t key : keys)
    bst.insert(key);

  cout << "BST construction complete.\n";


  // --------------------------------------------------------
  // Create disk-backed B-tree
  // --------------------------------------------------------

  cout << "Building disk-backed B-tree...\n";

  Disk btree_disk("btree.dat");

  DiskBTree btree(btree_disk);

  stats.reset();

  for (int64_t key : keys)
    btree.insert(key);

  cout << "B-tree construction complete.\n\n";


  // --------------------------------------------------------
  // Generate search keys
  // --------------------------------------------------------

  uniform_int_distribution<int64_t>
    distribution(0, N - 1);

  vector<int64_t> queries(NUM_SEARCHES);

  for (auto& q : queries)
    q = distribution(rng);


  // --------------------------------------------------------
  // Search BST
  // --------------------------------------------------------

  uint64_t bst_total_blocks = 0;
  uint64_t bst_total_comparisons = 0;

  uint64_t bst_max_blocks = 0;
  uint64_t bst_max_comparisons = 0;

  for (int64_t key : queries) {

    stats.reset();

    bool found = bst.search(key);

    assert(found);

    bst_total_blocks += stats.block_reads;
    bst_total_comparisons += stats.comparisons;

    bst_max_blocks =
      max(bst_max_blocks,
          stats.block_reads);

    bst_max_comparisons =
      max(bst_max_comparisons,
          stats.comparisons);
  }


  // --------------------------------------------------------
  // Search B-tree
  // --------------------------------------------------------

  uint64_t btree_total_blocks = 0;
  uint64_t btree_total_comparisons = 0;

  uint64_t btree_max_blocks = 0;
  uint64_t btree_max_comparisons = 0;

  for (int64_t key : queries) {

    stats.reset();

    bool found =
      btree.search(key);

    assert(found);

    btree_total_blocks +=
      stats.block_reads;

    btree_total_comparisons +=
      stats.comparisons;

    btree_max_blocks =
      max(btree_max_blocks,
          stats.block_reads);

    btree_max_comparisons =
      max(btree_max_comparisons,
          stats.comparisons);
  }


  // --------------------------------------------------------
  // Results
  // --------------------------------------------------------

  cout << "\n============================================\n";
  cout << "RESULTS\n";
  cout << "============================================\n\n";

  cout << "Average per search:\n\n";

  cout << "                    Block Reads    Comparisons\n";

  cout << "Binary Search Tree  "
    << (double)bst_total_blocks / NUM_SEARCHES
    << "          "
    << (double)bst_total_comparisons / NUM_SEARCHES
    << '\n';

  cout << "B-Tree              "
    << (double)btree_total_blocks / NUM_SEARCHES
    << "           "
    << (double)btree_total_comparisons / NUM_SEARCHES
    << "\n\n";

  return 0;
}
