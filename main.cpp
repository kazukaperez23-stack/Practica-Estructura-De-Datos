#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "AVLTree.hpp"
#include "Benchmark.hpp"
#include "HeapSort.hpp"
#include "QuickSort.hpp"
#include "Utils.hpp"

using Clock = std::chrono::high_resolution_clock;

static double elapsedMs(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

static size_t totalCharacters(const std::vector<std::string>& words) {
    size_t total = 0;
    for (const auto& w : words) total += w.size();
    return total;
}

static void printSectionTitle(const std::string& title) {
    std::cout << "\n" << title << "\n";
    std::cout << std::string(title.size(), '=') << "\n";
}

int main(int argc, char** argv) {
    const std::string datasetPath = (argc > 1) ? argv[1] : "dataset.txt";

    std::vector<std::string> original;
    try {
        original = ds::loadWords(datasetPath);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    const size_t n = original.size();
    std::cout << "Loaded " << n << " words from " << datasetPath << "\n";

    // =====================================================================
    // PART 1 - Las tres estrategias son correctas
    // =====================================================================
    printSectionTitle("1. Correctness verification");

    std::vector<std::string> quickSorted = original;
    auto t0 = Clock::now();
    ds::quickSort(quickSorted);
    auto t1 = Clock::now();
    double quickSortMs = elapsedMs(t0, t1);

    std::vector<std::string> heapSorted = original;
    t0 = Clock::now();
    ds::heapSort(heapSorted);
    t1 = Clock::now();
    double heapSortMs = elapsedMs(t0, t1);

    t0 = Clock::now();
    ds::AVLTree tree;
    for (const auto& w : original) tree.insert(w);
    std::vector<std::string> avlSorted = tree.inorder();
    t1 = Clock::now();
    double avlMs = elapsedMs(t0, t1);

    bool quickOk = ds::isSorted(quickSorted) && quickSorted.size() == n;
    bool heapOk = ds::isSorted(heapSorted) && heapSorted.size() == n;
    bool avlOk = ds::isSorted(avlSorted) && avlSorted.size() == tree.size();
    bool allMatch = (quickSorted == heapSorted) && (heapSorted == avlSorted);

    std::cout << std::boolalpha;
    std::cout << "QuickSort produced a sorted array of the expected size : " << quickOk << "\n";
    std::cout << "HeapSort produced a sorted array of the expected size  : " << heapOk << "\n";
    std::cout << "AVL inorder traversal is sorted (" << tree.size() << " unique keys) : " << avlOk << "\n";
    std::cout << "All three results are identical                       : " << allMatch << "\n";
    std::cout << std::noboolalpha;

    // =====================================================================
    // PART 2 - Memory usage estimation
    // =====================================================================
    printSectionTitle("2. Memory usage estimation");

    size_t chars = totalCharacters(original);
    size_t vectorContainerBytes = n * sizeof(std::string);
    size_t vectorTotalBytes = vectorContainerBytes + chars;
    size_t avlContainerBytes = tree.size() * sizeof(ds::AVLNode);
    size_t avlTotalBytes = avlContainerBytes + chars;

    std::cout << "n = " << n << " words, " << chars << " characters total\n";
    std::cout << "sizeof(std::string) = " << sizeof(std::string)
              << " bytes | sizeof(ds::AVLNode) = " << sizeof(ds::AVLNode)
              << " bytes (string + 2 pointers + int height)\n\n";

    std::cout << "vector<string>  (QuickSort / HeapSort)\n";
    std::cout << "  container (n * sizeof(string)) : " << ds::formatBytes((double)vectorContainerBytes) << "\n";
    std::cout << "  approx. character payload      : " << ds::formatBytes((double)chars) << "\n";
    std::cout << "  approx. total                  : " << ds::formatBytes((double)vectorTotalBytes) << "\n\n";

    std::cout << "AVL tree (" << tree.size() << " nodes, height = " << tree.treeHeight() << ")\n";
    std::cout << "  node overhead (n * sizeof(AVLNode)) : " << ds::formatBytes((double)avlContainerBytes) << "\n";
    std::cout << "  approx. character payload           : " << ds::formatBytes((double)chars) << "\n";
    std::cout << "  approx. total                       : " << ds::formatBytes((double)avlTotalBytes) << "\n";
    std::cout << "  extra overhead vs vector            : "
              << ds::formatBytes((double)avlTotalBytes - (double)vectorTotalBytes) << "\n";

    // =====================================================================
    // PART 3 - Experiment A: behaviour under different input scenarios
    // =====================================================================
    printSectionTitle("3. Experiment A - input scenarios (n = " + std::to_string(n) + ")");
    std::cout << "Methodology: each value is the mean of " << ds::BENCH_REPETITIONS
              << " runs. The input is copied before each run, so the copy cost\n"
              << "is paid outside the measured region. Ordered inputs are produced\n"
              << "with our own QuickSort, never with a library sort.\n\n";

    std::vector<ds::Measurement> results;
    const ds::Scenario scenarios[] = {ds::Scenario::Random, ds::Scenario::Sorted, ds::Scenario::Reversed};

    std::cout << std::fixed << std::setprecision(3);
    std::cout << std::left << std::setw(14) << "Scenario"
              << std::right << std::setw(14) << "QuickSort"
              << std::setw(14) << "HeapSort"
              << std::setw(14) << "AVL Tree" << "\n";
    std::cout << std::string(56, '-') << "\n";

    for (ds::Scenario sc : scenarios) {
        std::vector<std::string> input = ds::makeInput(original, sc, n);
        double q = ds::timeQuickSort(input);
        double h = ds::timeHeapSort(input);
        double a = ds::timeAVL(input);

        results.push_back({"scenario", ds::scenarioName(sc), n, "QuickSort", q});
        results.push_back({"scenario", ds::scenarioName(sc), n, "HeapSort", h});
        results.push_back({"scenario", ds::scenarioName(sc), n, "AVL Tree", a});

        std::cout << std::left << std::setw(14) << ds::scenarioName(sc)
                  << std::right << std::setw(14) << q
                  << std::setw(14) << h
                  << std::setw(14) << a << "\n";
    }
    std::cout << std::string(56, '-') << "\n";
    std::cout << "(times in milliseconds)\n";

    // =====================================================================
    // PART 4 - Experiment B: scalability (growth with n)
    // =====================================================================
    printSectionTitle("4. Experiment B - scalability on random data");
    std::cout << "If the implementations really behave as O(n log n), then t/(n log n)\n"
              << "should stay roughly constant as n grows.\n\n";

    const std::size_t sizes[] = {1000, 5000, 10000, 25000, 50000, 100000};

    std::cout << std::left << std::setw(10) << "n"
              << std::right << std::setw(13) << "QuickSort"
              << std::setw(13) << "HeapSort"
              << std::setw(13) << "AVL Tree"
              << "   |" << std::setw(11) << "Q/(nlogn)"
              << std::setw(11) << "H/(nlogn)"
              << std::setw(11) << "A/(nlogn)" << "\n";
    std::cout << std::string(83, '-') << "\n";

    for (std::size_t size : sizes) {
        if (size > n) break;
        std::vector<std::string> input = ds::makeInput(original, ds::Scenario::Random, size);
        double q = ds::timeQuickSort(input);
        double h = ds::timeHeapSort(input);
        double a = ds::timeAVL(input);

        results.push_back({"scalability", "Random", size, "QuickSort", q});
        results.push_back({"scalability", "Random", size, "HeapSort", h});
        results.push_back({"scalability", "Random", size, "AVL Tree", a});

        // Normalised cost: if the algorithm is really O(n log n), dividing the
        // measured time by n*log2(n) should give a roughly constant value.
        double nlogn = static_cast<double>(size) * std::log2(static_cast<double>(size));
        double scale = 1e6; // microseconds per (n log n) unit, for readability

        std::cout << std::left << std::setw(10) << size
                  << std::right << std::setw(13) << q
                  << std::setw(13) << h
                  << std::setw(13) << a
                  << "   |" << std::setw(11) << (q / nlogn * scale)
                  << std::setw(11) << (h / nlogn * scale)
                  << std::setw(11) << (a / nlogn * scale) << "\n";
    }
    std::cout << std::string(83, '-') << "\n";
    std::cout << "(times in milliseconds; normalised columns are t/(n*log2 n) x 1e6)\n";

    // =====================================================================
    // Outputs
    // =====================================================================
    std::ofstream csv("benchmark_results.csv");
    csv << "test_type,scenario,n,algorithm,time_ms\n";
    csv << std::fixed << std::setprecision(4);
    for (const auto& m : results) {
        csv << m.testType << "," << m.scenario << "," << m.n << ","
            << m.algorithm << "," << m.timeMs << "\n";
    }
    csv.close();

    std::ofstream out("sorted_dataset.txt");
    for (const auto& w : quickSorted) out << w << "\n";
    out.close();

    printSectionTitle("Files written");
    std::cout << "benchmark_results.csv : all measurements (for charting)\n";
    std::cout << "sorted_dataset.txt    : the " << n << " words in alphabetical order\n";

    std::cout << "\nSingle-run reference times (part 1): QuickSort " << quickSortMs
              << " ms | HeapSort " << heapSortMs << " ms | AVL " << avlMs << " ms\n";

    std::cout << "\nSample of final sorted result (first 5 / last 5 words):\n";
    for (size_t i = 0; i < 5 && i < quickSorted.size(); ++i) std::cout << "  " << quickSorted[i] << "\n";
    std::cout << "  ...\n";
    for (size_t i = quickSorted.size() >= 5 ? quickSorted.size() - 5 : 0; i < quickSorted.size(); ++i) {
        std::cout << "  " << quickSorted[i] << "\n";
    }

    return (quickOk && heapOk && avlOk && allMatch) ? 0 : 2;
}
