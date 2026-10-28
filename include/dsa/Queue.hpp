#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace medifind::dsa {

/**
 * @brief Custom FIFO Queue demonstrating First-In, First-Out request scheduling.
 * Ideal for managing purchase approvals and restock request processing pipelines.
 * Time Complexity:
 *   - enqueue: O(1)
 *   - dequeue: O(1)
 *   - front:   O(1)
 * Space Complexity: O(n)
 */
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    size_t size_;

public:
    Queue() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~Queue() {
        clear();
    }

    Queue(const Queue& other) : head_(nullptr), tail_(nullptr), size_(0) {
        Node* curr = other.head_;
        while (curr) {
            enqueue(curr->data);
            curr = curr->next;
        }
    }

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            clear();
            Node* curr = other.head_;
            while (curr) {
                enqueue(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    Queue(Queue&& other) noexcept : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    void clear() {
        Node* curr = head_;
        while (curr) {
            Node* temp = curr->next;
            delete curr;
            curr = temp;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        if (tail_) {
            tail_->next = newNode;
        } else {
            head_ = newNode;
        }
        tail_ = newNode;
        ++size_;
    }

    T dequeue() {
        if (empty()) throw std::out_of_range("Queue is empty");
        Node* temp = head_;
        T val = temp->data;
        head_ = head_->next;
        if (!head_) {
            tail_ = nullptr;
        }
        delete temp;
        --size_;
        return val;
    }

    T& front() {
        if (empty()) throw std::out_of_range("Queue is empty");
        return head_->data;
    }

    const T& front() const {
        if (empty()) throw std::out_of_range("Queue is empty");
        return head_->data;
    }

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }

    std::vector<T> toVector() const {
        std::vector<T> result;
        result.reserve(size_);
        Node* curr = head_;
        while (curr) {
            result.push_back(curr->data);
            curr = curr->next;
        }
        return result;
    }
};

} // namespace medifind::dsa
