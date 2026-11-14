#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <unordered_map>

// Custom Hash Table - Separate Chaining
// Medicine ID and barcode lookup
// Average lookup O(1)

template<typename K, typename V>
class HashTable {
private:
    struct Entry {
        K key;
        V value;
        bool occupied;
        Entry() : occupied(false) {}
    };

    std::vector<std::vector<Entry>> buckets;
    size_t numElements;
    size_t bucketCount;
    static const double MAX_LOAD_FACTOR;

    size_t hash(const K& key) const {
        return std::hash<K>{}(key) % bucketCount;
    }

    void rehash() {
        std::vector<std::vector<Entry>> oldBuckets = std::move(buckets);
        bucketCount *= 2;
        buckets.assign(bucketCount, {});
        numElements = 0;
        for (auto& chain : oldBuckets) {
            for (auto& entry : chain) {
                if (entry.occupied) {
                    insert(entry.key, entry.value);
                }
            }
        }
    }

public:
    HashTable(size_t initialCapacity = 16) : bucketCount(initialCapacity), numElements(0) {
        buckets.assign(bucketCount, {});
    }

    bool insert(const K& key, const V& value) {
        if ((double)(numElements + 1) / bucketCount > MAX_LOAD_FACTOR) {
            rehash();
        }
        size_t idx = hash(key);
        for (auto& entry : buckets[idx]) {
            if (entry.occupied && entry.key == key) {
                entry.value = value;
                return true;
            }
        }
        buckets[idx].push_back(Entry{key, value, true});
        numElements++;
        return true;
    }

    std::optional<V> search(const K& key) const {
        size_t idx = hash(key);
        for (const auto& entry : buckets[idx]) {
            if (entry.occupied && entry.key == key) {
                return entry.value;
            }
        }
        return std::nullopt;
    }

    bool remove(const K& key) {
        size_t idx = hash(key);
        auto& chain = buckets[idx];
        for (size_t i = 0; i < chain.size(); i++) {
            if (chain[i].occupied && chain[i].key == key) {
                chain[i].occupied = false;
                numElements--;
                return true;
            }
        }
        return false;
    }

    size_t size() const { return numElements; }
    bool empty() const { return numElements == 0; }
};

template<typename K, typename V>
const double HashTable<K, V>::MAX_LOAD_FACTOR = 0.75;
