#pragma once

#include <vector>
#include <functional>
#include <utility>
#include <cstddef>
#include <optional>

namespace medifind::dsa {

/**
 * @brief Algorithmic sorting and searching utilities.
 * Demonstrates:
 *   1. Merge Sort: Stable sorting with O(n log n) guaranteed time complexity.
 *   2. Quick Sort: In-place sorting with O(n log n) average time complexity.
 *   3. Binary Search: O(log n) lookup on ordered arrays.
 */
class Sorting {
public:
    // ==========================================
    // 1. MERGE SORT (Stable, O(n log n))
    // ==========================================
    template <typename T, typename Compare = std::less<T>>
    static void mergeSort(std::vector<T>& arr, Compare comp = Compare()) {
        if (arr.size() <= 1) return;
        std::vector<T> temp(arr.size());
        mergeSortInternal(arr, temp, 0, arr.size() - 1, comp);
    }

private:
    template <typename T, typename Compare>
    static void mergeSortInternal(std::vector<T>& arr, std::vector<T>& temp, size_t left, size_t right, Compare comp) {
        if (left >= right) return;
        size_t mid = left + (right - left) / 2;
        mergeSortInternal(arr, temp, left, mid, comp);
        mergeSortInternal(arr, temp, mid + 1, right, comp);
        merge(arr, temp, left, mid, right, comp);
    }

    template <typename T, typename Compare>
    static void merge(std::vector<T>& arr, std::vector<T>& temp, size_t left, size_t mid, size_t right, Compare comp) {
        size_t i = left;
        size_t j = mid + 1;
        size_t k = left;

        while (i <= mid && j <= right) {
            if (comp(arr[i], arr[j]) || (!comp(arr[j], arr[i]) && i <= mid)) {
                // Ensure stability by picking left element on equality
                temp[k++] = std::move(arr[i++]);
            } else {
                temp[k++] = std::move(arr[j++]);
            }
        }

        while (i <= mid) {
            temp[k++] = std::move(arr[i++]);
        }
        while (j <= right) {
            temp[k++] = std::move(arr[j++]);
        }

        for (size_t idx = left; idx <= right; ++idx) {
            arr[idx] = std::move(temp[idx]);
        }
    }

public:
    // ==========================================
    // 2. QUICK SORT (In-place, O(n log n) avg)
    // ==========================================
    template <typename T, typename Compare = std::less<T>>
    static void quickSort(std::vector<T>& arr, Compare comp = Compare()) {
        if (arr.size() <= 1) return;
        quickSortInternal(arr, 0, static_cast<int>(arr.size() - 1), comp);
    }

private:
    template <typename T, typename Compare>
    static void quickSortInternal(std::vector<T>& arr, int low, int high, Compare comp) {
        if (low < high) {
            int pi = partition(arr, low, high, comp);
            quickSortInternal(arr, low, pi - 1, comp);
            quickSortInternal(arr, pi + 1, high, comp);
        }
    }

    template <typename T, typename Compare>
    static int partition(std::vector<T>& arr, int low, int high, Compare comp) {
        // Median-of-three or last element as pivot
        int mid = low + (high - low) / 2;
        // Swap mid with high to improve pivot selection
        std::swap(arr[mid], arr[high]);
        const T& pivot = arr[high];

        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (comp(arr[j], pivot)) {
                ++i;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

public:
    // ==========================================
    // 3. BINARY SEARCH (O(log n))
    // ==========================================
    template <typename T, typename Key, typename KeyExtractor>
    static std::optional<size_t> binarySearch(const std::vector<T>& sortedArr, const Key& key, KeyExtractor extractor) {
        if (sortedArr.empty()) return std::nullopt;
        int low = 0;
        int high = static_cast<int>(sortedArr.size() - 1);

        while (low <= high) {
            int mid = low + (high - low) / 2;
            const Key& midVal = extractor(sortedArr[mid]);
            if (midVal == key) {
                return static_cast<size_t>(mid);
            } else if (midVal < key) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return std::nullopt;
    }
};

} // namespace medifind::dsa
