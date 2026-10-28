#pragma once

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <algorithm>
#include <utility>

namespace medifind::dsa {

/**
 * @brief Binary Search Tree (BST) for ordered keys and prefix searching.
 * Useful for fast prefix queries on medicine names and ordered catalogs.
 * Time Complexity:
 *   - Search/Insert: O(log n) average, O(n) worst-case (skewed tree)
 *   - In-order Traversal: O(n)
 * Space Complexity: O(n)
 */
template <typename Key, typename Value>
class BinarySearchTree {
public:
    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;

        Node(const Key& k, const Value& v)
            : key(k), value(v), left(nullptr), right(nullptr) {}
    };

private:
    Node* root_;
    size_t size_;

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    Node* copyTree(Node* node) {
        if (!node) return nullptr;
        Node* newNode = new Node(node->key, node->value);
        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);
        return newNode;
    }

    Node* insertNode(Node* node, const Key& key, const Value& value, bool& inserted) {
        if (!node) {
            inserted = true;
            return new Node(key, value);
        }
        if (key < node->key) {
            node->left = insertNode(node->left, key, value, inserted);
        } else if (node->key < key) {
            node->right = insertNode(node->right, key, value, inserted);
        } else {
            // Key already exists, update value
            node->value = value;
            inserted = false;
        }
        return node;
    }

    void inOrderHelper(Node* node, std::vector<std::pair<Key, Value>>& result) const {
        if (!node) return;
        inOrderHelper(node->left, result);
        result.emplace_back(node->key, node->value);
        inOrderHelper(node->right, result);
    }

    void prefixHelper(Node* node, const std::string& prefix, std::vector<std::pair<Key, Value>>& result) const {
        if (!node) return;

        // In C++, Key is typically std::string for prefix searching
        std::string keyStr = static_cast<std::string>(node->key);
        if (keyStr.rfind(prefix, 0) == 0) {
            // Matches prefix
            prefixHelper(node->left, prefix, result);
            result.emplace_back(node->key, node->value);
            prefixHelper(node->right, prefix, result);
        } else if (prefix < keyStr) {
            prefixHelper(node->left, prefix, result);
            // Even if this node does not match prefix, right subtree might have prefix matches if keyStr < prefix
        } else {
            prefixHelper(node->right, prefix, result);
        }
    }

    size_t heightHelper(Node* node) const {
        if (!node) return 0;
        return 1 + std::max(heightHelper(node->left), heightHelper(node->right));
    }

public:
    BinarySearchTree() : root_(nullptr), size_(0) {}

    ~BinarySearchTree() {
        destroy(root_);
    }

    BinarySearchTree(const BinarySearchTree& other) : root_(nullptr), size_(other.size_) {
        root_ = copyTree(other.root_);
    }

    BinarySearchTree& operator=(const BinarySearchTree& other) {
        if (this != &other) {
            destroy(root_);
            root_ = copyTree(other.root_);
            size_ = other.size_;
        }
        return *this;
    }

    BinarySearchTree(BinarySearchTree&& other) noexcept : root_(other.root_), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept {
        if (this != &other) {
            destroy(root_);
            root_ = other.root_;
            size_ = other.size_;
            other.root_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    bool insert(const Key& key, const Value& value) {
        bool inserted = false;
        root_ = insertNode(root_, key, value, inserted);
        if (inserted) ++size_;
        return inserted;
    }

    std::optional<Value> find(const Key& key) const {
        Node* curr = root_;
        while (curr) {
            if (key < curr->key) {
                curr = curr->left;
            } else if (curr->key < key) {
                curr = curr->right;
            } else {
                return curr->value;
            }
        }
        return std::nullopt;
    }

    bool contains(const Key& key) const {
        return find(key).has_value();
    }

    std::vector<std::pair<Key, Value>> inOrder() const {
        std::vector<std::pair<Key, Value>> result;
        result.reserve(size_);
        inOrderHelper(root_, result);
        return result;
    }

    std::vector<std::pair<Key, Value>> findByPrefix(const std::string& prefix) const {
        std::vector<std::pair<Key, Value>> result;
        // Simple in-order traversal with prefix filter for general safety
        auto all = inOrder();
        for (const auto& item : all) {
            std::string k = static_cast<std::string>(item.first);
            if (k.rfind(prefix, 0) == 0) {
                result.push_back(item);
            }
        }
        return result;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    size_t height() const { return heightHelper(root_); }
    void clear() {
        destroy(root_);
        root_ = nullptr;
        size_ = 0;
    }
};

} // namespace medifind::dsa
