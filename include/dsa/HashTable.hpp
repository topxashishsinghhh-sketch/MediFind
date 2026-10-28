#pragma once

#include <vector>
#include <string>
#include <functional>
#include <stdexcept>
#include <utility>
#include <optional>
#include <cstddef>

namespace medifind::dsa {

/**
 * @brief Custom Hash Table demonstrating separate-chaining collision resolution.
 * Time Complexity:
 *   - Insertion: O(1) average, O(n) worst-case (rehashing or extreme collision)
 *   - Lookup:    O(1) average, O(n) worst-case
 *   - Deletion:  O(1) average, O(n) worst-case
 * Space Complexity: O(n + m) where n is elements, m is bucket count
 */
template <typename Key, typename Value, typename Hash = std::hash<Key>>
class HashTable {
public:
    struct Entry {
        Key key;
        Value value;
        Entry* next;

        Entry(const Key& k, const Value& v, Entry* n = nullptr)
            : key(k), value(v), next(n) {}
    };

private:
    std::vector<Entry*> buckets_;
    size_t size_;
    float maxLoadFactor_;
    Hash hashFunc_;

    static const size_t INITIAL_CAPACITY = 17; // Prime number for better distribution

    size_t bucketIndex(const Key& key, size_t capacity) const {
        return hashFunc_(key) % capacity;
    }

    void rehash(size_t newCapacity) {
        std::vector<Entry*> newBuckets(newCapacity, nullptr);
        for (size_t i = 0; i < buckets_.size(); ++i) {
            Entry* curr = buckets_[i];
            while (curr != nullptr) {
                Entry* next = curr->next;
                size_t newIdx = bucketIndex(curr->key, newCapacity);
                curr->next = newBuckets[newIdx];
                newBuckets[newIdx] = curr;
                curr = next;
            }
        }
        buckets_ = std::move(newBuckets);
    }

public:
    explicit HashTable(size_t initialCapacity = INITIAL_CAPACITY, float maxLoadFactor = 0.75f)
        : buckets_(initialCapacity, nullptr), size_(0), maxLoadFactor_(maxLoadFactor) {}

    ~HashTable() {
        clear();
    }

    HashTable(const HashTable& other) : buckets_(other.buckets_.size(), nullptr), size_(0), maxLoadFactor_(other.maxLoadFactor_) {
        for (size_t i = 0; i < other.buckets_.size(); ++i) {
            Entry* curr = other.buckets_[i];
            while (curr) {
                insert(curr->key, curr->value);
                curr = curr->next;
            }
        }
    }

    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            clear();
            buckets_.assign(other.buckets_.size(), nullptr);
            maxLoadFactor_ = other.maxLoadFactor_;
            for (size_t i = 0; i < other.buckets_.size(); ++i) {
                Entry* curr = other.buckets_[i];
                while (curr) {
                    insert(curr->key, curr->value);
                    curr = curr->next;
                }
            }
        }
        return *this;
    }

    HashTable(HashTable&& other) noexcept
        : buckets_(std::move(other.buckets_)), size_(other.size_), maxLoadFactor_(other.maxLoadFactor_) {
        other.size_ = 0;
    }

    HashTable& operator=(HashTable&& other) noexcept {
        if (this != &other) {
            clear();
            buckets_ = std::move(other.buckets_);
            size_ = other.size_;
            maxLoadFactor_ = other.maxLoadFactor_;
            other.size_ = 0;
        }
        return *this;
    }

    void clear() {
        for (size_t i = 0; i < buckets_.size(); ++i) {
            Entry* curr = buckets_[i];
            while (curr != nullptr) {
                Entry* next = curr->next;
                delete curr;
                curr = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }

    bool insert(const Key& key, const Value& value) {
        if (static_cast<float>(size_ + 1) / buckets_.size() > maxLoadFactor_) {
            rehash(buckets_.size() * 2 + 1);
        }

        size_t idx = bucketIndex(key, buckets_.size());
        Entry* curr = buckets_[idx];
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value; // Update existing
                return false;
            }
            curr = curr->next;
        }

        buckets_[idx] = new Entry(key, value, buckets_[idx]);
        ++size_;
        return true;
    }

    std::optional<Value> find(const Key& key) const {
        size_t idx = bucketIndex(key, buckets_.size());
        Entry* curr = buckets_[idx];
        while (curr != nullptr) {
            if (curr->key == key) {
                return curr->value;
            }
            curr = curr->next;
        }
        return std::nullopt;
    }

    Value* findPtr(const Key& key) {
        size_t idx = bucketIndex(key, buckets_.size());
        Entry* curr = buckets_[idx];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    const Value* findPtr(const Key& key) const {
        size_t idx = bucketIndex(key, buckets_.size());
        Entry* curr = buckets_[idx];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool contains(const Key& key) const {
        return find(key).has_value();
    }

    bool remove(const Key& key) {
        size_t idx = bucketIndex(key, buckets_.size());
        Entry* curr = buckets_[idx];
        Entry* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr) {
                    buckets_[idx] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                --size_;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    size_t bucketCount() const { return buckets_.size(); }
    float loadFactor() const { return static_cast<float>(size_) / buckets_.size(); }

    std::vector<std::pair<Key, Value>> toVector() const {
        std::vector<std::pair<Key, Value>> result;
        result.reserve(size_);
        for (size_t i = 0; i < buckets_.size(); ++i) {
            Entry* curr = buckets_[i];
            while (curr != nullptr) {
                result.emplace_back(curr->key, curr->value);
                curr = curr->next;
            }
        }
        return result;
    }
};

} // namespace medifind::dsa
