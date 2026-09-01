#pragma once
#include <string>
#include <utility>
#include <vector>

// QuickSort from scratch over std::vector<string>.
// Pivot = median-of-three, switches to insertion sort below a size
// cutoff (classic optimization that also avoids the O(n^2) worst case
// on nearly-sorted small partitions and bounds recursion depth).
namespace ds {

constexpr int QUICKSORT_INSERTION_CUTOFF = 16;

inline void insertionSort(std::vector<std::string>& arr, int low, int high) {
    for (int i = low + 1; i <= high; ++i) {
        std::string key = arr[i];
        int j = i - 1;
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

inline int medianOfThreePivot(std::vector<std::string>& arr, int low, int high) {
    int mid = low + (high - low) / 2;
    if (arr[mid] < arr[low]) std::swap(arr[low], arr[mid]);
    if (arr[high] < arr[low]) std::swap(arr[low], arr[high]);
    if (arr[high] < arr[mid]) std::swap(arr[mid], arr[high]);
    // Pivot parked at high-1 so both scan ends have a natural sentinel.
    std::swap(arr[mid], arr[high - 1]);
    return high - 1;
}

inline int partition(std::vector<std::string>& arr, int low, int high) {
    int pivotIndex = medianOfThreePivot(arr, low, high);
    const std::string pivot = arr[pivotIndex];

    int i = low;
    int j = high - 1;
    while (true) {
        while (arr[++i] < pivot) {}
        while (pivot < arr[--j]) {}
        if (i >= j) break;
        std::swap(arr[i], arr[j]);
    }
    std::swap(arr[i], arr[high - 1]);
    return i;
}

inline void quickSortRec(std::vector<std::string>& arr, int low, int high) {
    if (high - low < QUICKSORT_INSERTION_CUTOFF) {
        insertionSort(arr, low, high);
        return;
    }
    int pivotIndex = partition(arr, low, high);
    quickSortRec(arr, low, pivotIndex - 1);
    quickSortRec(arr, pivotIndex + 1, high);
}

inline void quickSort(std::vector<std::string>& arr) {
    if (arr.size() < 2) return;
    quickSortRec(arr, 0, static_cast<int>(arr.size()) - 1);
}

} // namespace ds
