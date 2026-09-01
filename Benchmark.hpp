#pragma once
#include <chrono>
#include <string>
#include <vector>

#include "AVLTree.hpp"
#include "HeapSort.hpp"
#include "QuickSort.hpp"

// Experimental harness: measures each strategy under several input scenarios
// and several input sizes, so the empirical results can be contrasted with the
// theoretical Big-O behaviour.
namespace ds {

using Clock = std::chrono::high_resolution_clock;

// How many times each measurement is repeated; the reported value is the mean.
constexpr int BENCH_REPETITIONS = 3;

enum class Scenario { Random, Sorted, Reversed };

inline const char* scenarioName(Scenario s) {
    switch (s) {
        case Scenario::Random:   return "Random";
        case Scenario::Sorted:   return "Sorted";
        case Scenario::Reversed: return "Reversed";
    }
    return "Unknown";
}

// Reverses a vector in place (std::reverse avoided so that no standard
// algorithm is involved anywhere near the sorting experiments).
inline void reverseInPlace(std::vector<std::string>& v) {
    if (v.empty()) return;
    for (std::size_t i = 0, j = v.size() - 1; i < j; ++i, --j) {
        std::swap(v[i], v[j]);
    }
}

// Builds the input for one scenario, using the first n words of the dataset.
// The ordered variants are produced with our own QuickSort.
inline std::vector<std::string> makeInput(const std::vector<std::string>& base,
                                          Scenario scenario,
                                          std::size_t n) {
    std::vector<std::string> data(base.begin(), base.begin() + n);
    if (scenario == Scenario::Sorted) {
        quickSort(data);
    } else if (scenario == Scenario::Reversed) {
        quickSort(data);
        reverseInPlace(data);
    }
    return data;
}

// Each timer receives its own copy of the input, so the cost of copying is
// paid outside the measured region.
inline double timeQuickSort(const std::vector<std::string>& input) {
    double total = 0.0;
    for (int r = 0; r < BENCH_REPETITIONS; ++r) {
        std::vector<std::string> data = input;
        auto start = Clock::now();
        quickSort(data);
        auto end = Clock::now();
        total += std::chrono::duration<double, std::milli>(end - start).count();
    }
    return total / BENCH_REPETITIONS;
}

inline double timeHeapSort(const std::vector<std::string>& input) {
    double total = 0.0;
    for (int r = 0; r < BENCH_REPETITIONS; ++r) {
        std::vector<std::string> data = input;
        auto start = Clock::now();
        heapSort(data);
        auto end = Clock::now();
        total += std::chrono::duration<double, std::milli>(end - start).count();
    }
    return total / BENCH_REPETITIONS;
}

// Measures the full AVL strategy: build the tree by insertion, then extract
// the ordering with an inorder traversal.
inline double timeAVL(const std::vector<std::string>& input) {
    double total = 0.0;
    for (int r = 0; r < BENCH_REPETITIONS; ++r) {
        auto start = Clock::now();
        AVLTree tree;
        for (const auto& w : input) tree.insert(w);
        std::vector<std::string> sorted = tree.inorder();
        auto end = Clock::now();
        total += std::chrono::duration<double, std::milli>(end - start).count();
    }
    return total / BENCH_REPETITIONS;
}

struct Measurement {
    std::string testType;   // "scenario" or "scalability"
    std::string scenario;
    std::size_t n;
    std::string algorithm;
    double timeMs;
};

} // namespace ds
