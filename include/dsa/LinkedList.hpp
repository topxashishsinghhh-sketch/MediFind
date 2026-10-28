#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>
#include <functional>

namespace medifind::dsa {

/**
 * @brief Custom Doubly-Linked List demonstrating non-contiguous node storage,
 * pointer manipulation, and forward/reverse traversal.
 * Time Complexity:
 *   - push_front / push_back: O(1)
 *   - pop_front / pop_back:   O(1)
 *   - access by index:        O(n)
 * Space Complexity: O(n)
 */
template <typename T>
class LinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;

        explicit Node(const T& val, Node* p = nullptr, Node* n = nullptr)
            : data(val), prev(p), next(n) {}
        explicit Node(T&& val, Node* p = nullptr, Node* n = nullptr)
            : data(std::move(val)), prev(p), next(n) {}
    };

private:
    Node* head_;
    Node* tail_;
    size_t size_;

public:
    LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    ~LinkedList() {
        clear();
    }

    LinkedList(const LinkedList& other) : head_(nullptr), tail_(nullptr), size_(0) {
        Node* curr = other.head_;
        while (curr) {
            push_back(curr->data);
            curr = curr->next;
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* curr = other.head_;
            while (curr) {
                push_back(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    LinkedList(LinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
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
        while (curr != nullptr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    void push_front(const T& val) {
        Node* newNode = new Node(val, nullptr, head_);
        if (head_) {
            head_->prev = newNode;
        } else {
            tail_ = newNode;
        }
        head_ = newNode;
        ++size_;
    }

    void push_back(const T& val) {
        Node* newNode = new Node(val, tail_, nullptr);
        if (tail_) {
            tail_->next = newNode;
        } else {
            head_ = newNode;
        }
        tail_ = newNode;
        ++size_;
    }

    void pop_front() {
        if (empty()) throw std::out_of_range("LinkedList is empty");
        Node* temp = head_;
        head_ = head_->next;
        if (head_) {
            head_->prev = nullptr;
        } else {
            tail_ = nullptr;
        }
        delete temp;
        --size_;
    }

    void pop_back() {
        if (empty()) throw std::out_of_range("LinkedList is empty");
        Node* temp = tail_;
        tail_ = tail_->prev;
        if (tail_) {
            tail_->next = nullptr;
        } else {
            head_ = nullptr;
        }
        delete temp;
        --size_;
    }

    T& front() {
        if (empty()) throw std::out_of_range("LinkedList is empty");
        return head_->data;
    }

    const T& front() const {
        if (empty()) throw std::out_of_range("LinkedList is empty");
        return head_->data;
    }

    T& back() {
        if (empty()) throw std::out_of_range("LinkedList is empty");
        return tail_->data;
    }

    const T& back() const {
        if (empty()) throw std::out_of_range("LinkedList is empty");
        return tail_->data;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    Node* head() const { return head_; }
    Node* tail() const { return tail_; }

    std::vector<T> toVector() const {
        std::vector<T> vec;
        vec.reserve(size_);
        Node* curr = head_;
        while (curr != nullptr) {
            vec.push_back(curr->data);
            curr = curr->next;
        }
        return vec;
    }

    void forEach(const std::function<void(const T&)>& func) const {
        Node* curr = head_;
        while (curr != nullptr) {
            func(curr->data);
            curr = curr->next;
        }
    }
};

} // namespace medifind::dsa
