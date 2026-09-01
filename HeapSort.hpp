#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Binary Heap structure + HeapSort, both from scratch.
//
// The heap is a max-heap stored implicitly in a dynamic array: the node at
// index i keeps its children at 2i+1 and 2i+2 and its parent at (i-1)/2, so
// the tree shape is encoded in the indices and no child pointers are needed.
namespace ds {

class BinaryHeap {
public:
    BinaryHeap() = default;

    // Builds the heap from an existing array in O(n) (bottom-up heapify).
    // Takes the vector by value + move so no copy of the data is made.
    explicit BinaryHeap(std::vector<std::string> items) : heap_(std::move(items)) {
        buildMaxHeap();
    }

    // O(log n): append at the end, then bubble the new element up.
    void insert(const std::string& value) {
        heap_.push_back(value);
        siftUp(heap_.size() - 1);
    }

    // O(log n): the root is the maximum; replace it with the last element
    // and sift that one down to restore the heap property.
    std::string extractMax() {
        if (heap_.empty()) throw std::out_of_range("extractMax() on an empty heap");
        std::string top = std::move(heap_[0]);
        heap_[0] = std::move(heap_.back());
        heap_.pop_back();
        if (!heap_.empty()) siftDown(0, heap_.size());
        return top;
    }

    const std::string& peekMax() const {
        if (heap_.empty()) throw std::out_of_range("peekMax() on an empty heap");
        return heap_[0];
    }

    // HeapSort itself: repeatedly swap the maximum into its final position at
    // the back and shrink the heap by one. Runs inside the same array, so it
    // needs no auxiliary storage (O(1) extra space).
    void sortAscending() {
        for (std::size_t end = heap_.size(); end > 1; --end) {
            std::swap(heap_[0], heap_[end - 1]);
            siftDown(0, end - 1);
        }
    }

    std::size_t size() const { return heap_.size(); }
    bool empty() const { return heap_.empty(); }
    const std::vector<std::string>& data() const { return heap_; }

    // Hands the underlying array back out without copying it.
    std::vector<std::string> release() { return std::move(heap_); }

private:
    std::vector<std::string> heap_;

    void buildMaxHeap() {
        // Leaves are already valid heaps, so start at the last internal node.
        for (std::size_t i = heap_.size() / 2; i-- > 0;) {
            siftDown(i, heap_.size());
        }
    }

    void siftUp(std::size_t i) {
        while (i > 0) {
            std::size_t parent = (i - 1) / 2;
            if (!(heap_[parent] < heap_[i])) break;
            std::swap(heap_[parent], heap_[i]);
            i = parent;
        }
    }

    void siftDown(std::size_t i, std::size_t heapSize) {
        while (true) {
            std::size_t left = 2 * i + 1;
            std::size_t right = 2 * i + 2;
            std::size_t largest = i;
            if (left < heapSize && heap_[largest] < heap_[left]) largest = left;
            if (right < heapSize && heap_[largest] < heap_[right]) largest = right;
            if (largest == i) return;
            std::swap(heap_[i], heap_[largest]);
            i = largest;
        }
    }
};

// Sorts the array using the BinaryHeap structure above.
// The vector is moved into the heap and moved back out, so the whole sort
// happens in one buffer without copying the 100,000 strings.
inline void heapSort(std::vector<std::string>& arr) {
    if (arr.size() < 2) return;
    BinaryHeap heap(std::move(arr)); // O(n) build
    heap.sortAscending();            // n x (extract max + siftDown) = O(n log n)
    arr = heap.release();
}

} // namespace ds
