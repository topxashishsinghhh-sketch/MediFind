#include <vector>
#include <optional>
#include <string>

// Custom Min Heap - Prioritise urgent expiry and stock alerts
// Insert and remove-min O(log n)

struct AlertItem {
    int priority;       // lower = more urgent
    int entityId;
    std::string message;
    std::string type;   // expiry, low_stock

    bool operator>(const AlertItem& other) const {
        return priority > other.priority;
    }
};

class MinHeap {
private:
    std::vector<AlertItem> heap;

    void siftUp(size_t idx) {
        while (idx > 0) {
            size_t parent = (idx - 1) / 2;
            if (heap[idx] > heap[parent]) break;
            std::swap(heap[idx], heap[parent]);
            idx = parent;
        }
    }

    void siftDown(size_t idx) {
        size_t n = heap.size();
        while (true) {
            size_t left = 2 * idx + 1;
            size_t right = 2 * idx + 2;
            size_t smallest = idx;
            if (left < n && heap[left] > heap[smallest]) smallest = left;
            if (right < n && heap[right] > heap[smallest]) smallest = right;
            if (smallest == idx) break;
            std::swap(heap[idx], heap[smallest]);
            idx = smallest;
        }
    }

public:
    void insert(int priority, int entityId, const std::string& message, const std::string& type) {
        heap.push_back(AlertItem{priority, entityId, message, type});
        siftUp(heap.size() - 1);
    }

    std::optional<AlertItem> removeMin() {
        if (heap.empty()) return std::nullopt;
        AlertItem min = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) siftDown(0);
        return min;
    }

    std::optional<AlertItem> peek() const {
        if (heap.empty()) return std::nullopt;
        return heap[0];
    }

    size_t size() const { return heap.size(); }
    bool empty() const { return heap.empty(); }
};
