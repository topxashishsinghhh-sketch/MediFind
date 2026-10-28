#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace medifind::dsa {

/**
 * @brief Custom LIFO Stack demonstrating Last-In, First-Out history and undo tracking.
 * Used for maintaining audit trail breadcrumbs, menu navigation history, and safe undo tokens.
 * Time Complexity:
 *   - push: O(1)
 *   - pop:  O(1)
 *   - top:  O(1)
 * Space Complexity: O(n)
 */
template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& val, Node* n = nullptr) : data(val), next(n) {}
    };

    Node* top_;
    size_t size_;

public:
    Stack() : top_(nullptr), size_(0) {}

    ~Stack() {
        clear();
    }

    Stack(const Stack& other) : top_(nullptr), size_(0) {
        // Deep copy preserving stack order
        if (!other.top_) return;
        std::vector<T> items;
        Node* curr = other.top_;
        while (curr) {
            items.push_back(curr->data);
            curr = curr->next;
        }
        for (auto it = items.rbegin(); it != items.rend(); ++it) {
            push(*it);
        }
    }

    Stack& operator=(const Stack& other) {
        if (this != &other) {
            clear();
            if (other.top_) {
                std::vector<T> items;
                Node* curr = other.top_;
                while (curr) {
                    items.push_back(curr->data);
                    curr = curr->next;
                }
                for (auto it = items.rbegin(); it != items.rend(); ++it) {
                    push(*it);
                }
            }
        }
        return *this;
    }

    Stack(Stack&& other) noexcept : top_(other.top_), size_(other.size_) {
        other.top_ = nullptr;
        other.size_ = 0;
    }

    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            clear();
            top_ = other.top_;
            size_ = other.size_;
            other.top_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    void clear() {
        Node* curr = top_;
        while (curr) {
            Node* temp = curr->next;
            delete curr;
            curr = temp;
        }
        top_ = nullptr;
        size_ = 0;
    }

    void push(const T& val) {
        top_ = new Node(val, top_);
        ++size_;
    }

    T pop() {
        if (empty()) throw std::out_of_range("Stack is empty");
        Node* temp = top_;
        T val = temp->data;
        top_ = top_->next;
        delete temp;
        --size_;
        return val;
    }

    T& top() {
        if (empty()) throw std::out_of_range("Stack is empty");
        return top_->data;
    }

    const T& top() const {
        if (empty()) throw std::out_of_range("Stack is empty");
        return top_->data;
    }

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
};

} // namespace medifind::dsa
