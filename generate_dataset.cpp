// Builds dataset.txt from words_alpha.txt.
//
// words_alpha.txt is already in alphabetical order, which would be the best
// case for some algorithms and the O(n^2) worst case for a naive QuickSort,
// so the words are shuffled first and only then the first N are taken.
//
// Usage:  generate_dataset [input] [output] [count]
// Default: generate_dataset words_alpha.txt dataset.txt 100000
//
// Note: std::shuffle is a permutation utility, not a sorting function -- the
// practice only forbids built-in sorting (std::sort) in the sorting stage.

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    const std::string inputPath = (argc > 1) ? argv[1] : "words_alpha.txt";
    const std::string outputPath = (argc > 2) ? argv[2] : "dataset.txt";
    const std::size_t count = (argc > 3) ? std::stoul(argv[3]) : 100000;

    std::ifstream in(inputPath);
    if (!in.is_open()) {
        std::cerr << "Error: could not open input file: " << inputPath << "\n";
        std::cerr << "Download words_alpha.txt and place it next to this program.\n";
        return 1;
    }

    std::vector<std::string> words;
    words.reserve(400000);
    std::string line;
    while (std::getline(in, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (!line.empty()) words.push_back(line);
    }
    in.close();

    std::cout << "Read " << words.size() << " words from " << inputPath << "\n";

    if (words.size() < count) {
        std::cerr << "Error: the input only has " << words.size()
                  << " words, but " << count << " were requested.\n";
        return 1;
    }

    // Mersenne Twister seeded from the OS entropy source, so every run
    // produces a different random selection.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(words.begin(), words.end(), gen);

    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "Error: could not write to " << outputPath << "\n";
        return 1;
    }
    for (std::size_t i = 0; i < count; ++i) {
        out << words[i] << "\n";
    }
    out.close();

    std::cout << "Wrote " << count << " randomly selected words to " << outputPath << "\n";
    return 0;
}
