#pragma once
#include <algorithm>
#include <string>
#include <vector>

// Self-balancing AVL tree. Insert keeps |balance factor| <= 1 at every
// node via rotations, which bounds height to O(log n) and therefore
// bounds insertion and the inorder traversal to O(n log n) / O(n).
namespace ds {

struct AVLNode {
    std::string key;
    AVLNode* left;
    AVLNode* right;
    int height;

    explicit AVLNode(const std::string& k)
        : key(k), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
public:
    AVLTree() : root(nullptr), nodeCount(0) {}
    ~AVLTree() { destroy(root); }

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    void insert(const std::string& key) { root = insertRec(root, key); }

    std::vector<std::string> inorder() const {
        std::vector<std::string> result;
        result.reserve(nodeCount);
        inorderRec(root, result);
        return result;
    }

    size_t size() const { return nodeCount; }
    int treeHeight() const { return height(root); }

private:
    AVLNode* root;
    size_t nodeCount;

    static int height(AVLNode* n) { return n ? n->height : 0; }

    static int balanceFactor(AVLNode* n) {
        return n ? height(n->left) - height(n->right) : 0;
    }

    static void updateHeight(AVLNode* n) {
        n->height = 1 + std::max(height(n->left), height(n->right));
    }

    static AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* t2 = x->right;
        x->right = y;
        y->left = t2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    static AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* t2 = y->left;
        y->left = x;
        x->right = t2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    AVLNode* insertRec(AVLNode* node, const std::string& key) {
        if (!node) {
            ++nodeCount;
            return new AVLNode(key);
        }
        if (key < node->key) {
            node->left = insertRec(node->left, key);
        } else if (key > node->key) {
            node->right = insertRec(node->right, key);
        } else {
            return node; // duplicate word: tree keeps a single copy
        }

        updateHeight(node);
        int balance = balanceFactor(node);

        if (balance > 1 && key < node->left->key) return rotateRight(node);
        if (balance < -1 && key > node->right->key) return rotateLeft(node);
        if (balance > 1 && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    void inorderRec(AVLNode* node, std::vector<std::string>& result) const {
        if (!node) return;
        inorderRec(node->left, result);
        result.push_back(node->key);
        inorderRec(node->right, result);
    }

    void destroy(AVLNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
};

} // namespace ds
