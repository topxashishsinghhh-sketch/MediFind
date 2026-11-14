#include <string>
#include <optional>
#include <vector>

// Custom Binary Search Tree - Batch expiry-date indexing
// In-order traversal for expiry ordering (FEFO)

struct BatchNode {
    int batchId;
    std::string expiryDate; // YYYY-MM-DD format, lexicographically comparable
    int productId;
    BatchNode* left;
    BatchNode* right;

    BatchNode(int bid, const std::string& exp, int pid)
        : batchId(bid), expiryDate(exp), productId(pid), left(nullptr), right(nullptr) {}
};

class ExpiryBST {
private:
    BatchNode* root;

    void destroyTree(BatchNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    BatchNode* insertNode(BatchNode* node, int batchId, const std::string& expiryDate, int productId) {
        if (!node) return new BatchNode(batchId, expiryDate, productId);
        if (expiryDate < node->expiryDate) {
            node->left = insertNode(node->left, batchId, expiryDate, productId);
        } else {
            node->right = insertNode(node->right, batchId, expiryDate, productId);
        }
        return node;
    }

    BatchNode* searchNode(BatchNode* node, const std::string& expiryDate) const {
        if (!node) return nullptr;
        if (expiryDate == node->expiryDate) return node;
        if (expiryDate < node->expiryDate) return searchNode(node->left, expiryDate);
        return searchNode(node->right, expiryDate);
    }

    void inorder(BatchNode* node, std::vector<BatchNode*>& result) const {
        if (!node) return;
        inorder(node->left, result);
        result.push_back(node);
        inorder(node->right, result);
    }

    BatchNode* findMin(BatchNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    BatchNode* removeNode(BatchNode* node, const std::string& expiryDate) {
        if (!node) return nullptr;
        if (expiryDate < node->expiryDate) {
            node->left = removeNode(node->left, expiryDate);
        } else if (expiryDate > node->expiryDate) {
            node->right = removeNode(node->right, expiryDate);
        } else {
            if (!node->left) {
                BatchNode* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                BatchNode* temp = node->left;
                delete node;
                return temp;
            }
            BatchNode* temp = findMin(node->right);
            node->expiryDate = temp->expiryDate;
            node->batchId = temp->batchId;
            node->productId = temp->productId;
            node->right = removeNode(node->right, temp->expiryDate);
        }
        return node;
    }

public:
    ExpiryBST() : root(nullptr) {}
    ~ExpiryBST() { destroyTree(root); }

    void insert(int batchId, const std::string& expiryDate, int productId) {
        root = insertNode(root, batchId, expiryDate, productId);
    }

    std::optional<int> search(const std::string& expiryDate) const {
        BatchNode* node = searchNode(root, expiryDate);
        if (node) return node->batchId;
        return std::nullopt;
    }

    void remove(const std::string& expiryDate) {
        root = removeNode(root, expiryDate);
    }

    // FEFO ordering: earliest expiry first
    std::vector<BatchNode*> getExpiryOrder() const {
        std::vector<BatchNode*> result;
        inorder(root, result);
        return result;
    }

    bool empty() const { return root == nullptr; }
};
