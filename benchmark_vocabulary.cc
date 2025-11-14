// Standalone vocabulary benchmark
// Compares bitset-based vocabulary implementation with baseline unordered_map

#include "src/core/jsonschema/include/sourcemeta/core/jsonschema_vocabularies.h"
#include "src/core/json/include/sourcemeta/core/json.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <vector>

using namespace sourcemeta::core;

// Baseline: Original unordered_map implementation
using VocabulariesBaseline = std::unordered_map<std::string, bool>;

// Test vocabularies (mix of known and custom)
const std::vector<std::pair<std::string, bool>> TEST_VOCABULARIES = {
    // Known vocabularies
    {"https://json-schema.org/draft/2020-12/vocab/core", true},
    {"https://json-schema.org/draft/2020-12/vocab/applicator", true},
    {"https://json-schema.org/draft/2020-12/vocab/validation", true},
    {"https://json-schema.org/draft/2020-12/vocab/meta-data", true},
    {"https://json-schema.org/draft/2019-09/vocab/core", true},
    {"https://json-schema.org/draft/2019-09/vocab/applicator", true},
    {"http://json-schema.org/draft-07/schema#", true},
    {"http://json-schema.org/draft-06/schema#", true},
    {"http://json-schema.org/draft-04/schema#", true},
    // Custom vocabularies
    {"https://example.com/custom/vocab1", true},
    {"https://example.com/custom/vocab2", false},
    {"https://my-org.com/schemas/v1", true}
};

// Frequently looked up vocabularies (80% hits)
const std::vector<std::string> LOOKUP_URIS = {
    "https://json-schema.org/draft/2020-12/vocab/core",
    "https://json-schema.org/draft/2020-12/vocab/applicator",
    "https://json-schema.org/draft/2020-12/vocab/validation",
    "https://json-schema.org/draft/2019-09/vocab/core",
    "http://json-schema.org/draft-07/schema#",
    "https://json-schema.org/draft/2020-12/vocab/core", // duplicate for cache effects
    "https://example.com/custom/vocab1",
    "https://json-schema.org/draft/2020-12/vocab/meta-data",
    "http://json-schema.org/draft-04/schema#",
    "https://json-schema.org/draft/2019-09/vocab/applicator"
};

template <typename Func>
double benchmark(const std::string& name, Func func, int iterations = 100000) {
    auto start = std::chrono::high_resolution_clock::now();
    func(iterations);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << std::setw(50) << std::left << name
              << std::setw(12) << std::right << std::fixed << std::setprecision(3)
              << ms << " ms"
              << std::setw(15) << std::right << (ms / iterations * 1000000) << " ns/op"
              << std::endl;
    return ms;
}

void benchmark_insert() {
    std::cout << "\n=== INSERT BENCHMARK ===" << std::endl;

    double baseline_time = benchmark("Baseline (unordered_map) insert", [](int iterations) {
        for (int i = 0; i < iterations; i++) {
            VocabulariesBaseline vocabs;
            for (const auto& entry : TEST_VOCABULARIES) {
                vocabs.insert(entry);
            }
        }
    });

    double optimized_time = benchmark("Optimized (bitset) insert", [](int iterations) {
        for (int i = 0; i < iterations; i++) {
            Vocabularies vocabs;
            for (const auto& entry : TEST_VOCABULARIES) {
                vocabs.insert(entry);
            }
        }
    });

    double improvement = ((baseline_time - optimized_time) / baseline_time) * 100.0;
    std::cout << "Improvement: " << std::fixed << std::setprecision(1)
              << improvement << "%" << std::endl;
}

void benchmark_lookup() {
    std::cout << "\n=== LOOKUP BENCHMARK (contains) ===" << std::endl;

    // Prepare data
    VocabulariesBaseline baseline;
    for (const auto& entry : TEST_VOCABULARIES) {
        baseline.insert(entry);
    }

    Vocabularies optimized;
    for (const auto& entry : TEST_VOCABULARIES) {
        optimized.insert(entry);
    }

    double baseline_time = benchmark("Baseline (unordered_map) contains", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            for (const auto& uri : LOOKUP_URIS) {
                auto it = baseline.find(uri);
                result = (it != baseline.end() && it->second);
            }
        }
    });

    double optimized_time = benchmark("Optimized (bitset) contains", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            for (const auto& uri : LOOKUP_URIS) {
                result = optimized.contains(uri);
            }
        }
    });

    double improvement = ((baseline_time - optimized_time) / baseline_time) * 100.0;
    std::cout << "Improvement: " << std::fixed << std::setprecision(1)
              << improvement << "%" << std::endl;
}

void benchmark_find() {
    std::cout << "\n=== FIND BENCHMARK (with status check) ===" << std::endl;

    // Prepare data
    VocabulariesBaseline baseline;
    for (const auto& entry : TEST_VOCABULARIES) {
        baseline.insert(entry);
    }

    Vocabularies optimized;
    for (const auto& entry : TEST_VOCABULARIES) {
        optimized.insert(entry);
    }

    double baseline_time = benchmark("Baseline (unordered_map) find", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            for (const auto& uri : LOOKUP_URIS) {
                auto it = baseline.find(uri);
                if (it != baseline.end()) {
                    result = it->second;
                }
            }
        }
    });

    double optimized_time = benchmark("Optimized (bitset) find", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            for (const auto& uri : LOOKUP_URIS) {
                auto status = optimized.find(uri);
                if (status.has_value()) {
                    result = status.value();
                }
            }
        }
    });

    double improvement = ((baseline_time - optimized_time) / baseline_time) * 100.0;
    std::cout << "Improvement: " << std::fixed << std::setprecision(1)
              << improvement << "%" << std::endl;
}

void benchmark_merge() {
    std::cout << "\n=== MERGE BENCHMARK ===" << std::endl;

    double baseline_time = benchmark("Baseline (unordered_map) merge", [](int iterations) {
        for (int i = 0; i < iterations; i++) {
            VocabulariesBaseline vocabs1;
            VocabulariesBaseline vocabs2;

            for (size_t j = 0; j < TEST_VOCABULARIES.size() / 2; j++) {
                vocabs1.insert(TEST_VOCABULARIES[j]);
            }
            for (size_t j = TEST_VOCABULARIES.size() / 2; j < TEST_VOCABULARIES.size(); j++) {
                vocabs2.insert(TEST_VOCABULARIES[j]);
            }

            vocabs1.merge(vocabs2);
        }
    });

    double optimized_time = benchmark("Optimized (bitset) merge", [](int iterations) {
        for (int i = 0; i < iterations; i++) {
            Vocabularies vocabs1;
            Vocabularies vocabs2;

            for (size_t j = 0; j < TEST_VOCABULARIES.size() / 2; j++) {
                vocabs1.insert(TEST_VOCABULARIES[j]);
            }
            for (size_t j = TEST_VOCABULARIES.size() / 2; j < TEST_VOCABULARIES.size(); j++) {
                vocabs2.insert(TEST_VOCABULARIES[j]);
            }

            vocabs1.merge(vocabs2);
        }
    });

    double improvement = ((baseline_time - optimized_time) / baseline_time) * 100.0;
    std::cout << "Improvement: " << std::fixed << std::setprecision(1)
              << improvement << "%" << std::endl;
}

void verify_correctness() {
    std::cout << "\n=== CORRECTNESS VERIFICATION ===" << std::endl;

    Vocabularies vocabs;

    // Test insert and contains
    vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/core", true});
    vocabs.insert({"https://json-schema.org/draft/2019-09/vocab/format", false});
    vocabs.insert({"https://example.com/custom", true});

    std::cout << "✓ Insert: 3 vocabularies added" << std::endl;

    // Test contains
    bool test1 = vocabs.contains("https://json-schema.org/draft/2020-12/vocab/core");
    bool test2 = !vocabs.contains("https://json-schema.org/draft/2019-09/vocab/format");
    bool test3 = vocabs.contains("https://example.com/custom");
    bool test4 = !vocabs.contains("https://non-existent.com/vocab");

    if (test1 && test2 && test3 && test4) {
        std::cout << "✓ Contains: All lookups correct" << std::endl;
    } else {
        std::cout << "✗ Contains: FAILED" << std::endl;
    }

    // Test find
    auto found1 = vocabs.find("https://json-schema.org/draft/2020-12/vocab/core");
    auto found2 = vocabs.find("https://json-schema.org/draft/2019-09/vocab/format");
    auto found3 = vocabs.find("https://non-existent.com/vocab");

    bool find_test = found1.has_value() && found1.value() == true &&
                     found2.has_value() && found2.value() == false &&
                     !found3.has_value();

    if (find_test) {
        std::cout << "✓ Find: All queries correct" << std::endl;
    } else {
        std::cout << "✗ Find: FAILED" << std::endl;
    }

    // Test merge
    Vocabularies vocabs2;
    vocabs2.insert({"https://json-schema.org/draft/2020-12/vocab/applicator", true});
    vocabs2.insert({"https://example.com/custom2", false});

    vocabs.merge(vocabs2);

    bool merge_test = vocabs.contains("https://json-schema.org/draft/2020-12/vocab/applicator") &&
                      !vocabs.contains("https://example.com/custom2");

    if (merge_test) {
        std::cout << "✓ Merge: Vocabularies merged correctly" << std::endl;
    } else {
        std::cout << "✗ Merge: FAILED" << std::endl;
    }

    // Test all_vocabularies
    auto all = vocabs.all_vocabularies();
    std::cout << "✓ All vocabularies: " << all.size() << " entries" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  VOCABULARY OPTIMIZATION BENCHMARK" << std::endl;
    std::cout << "========================================" << std::endl;

    verify_correctness();
    benchmark_insert();
    benchmark_lookup();
    benchmark_find();
    benchmark_merge();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  BENCHMARK COMPLETE" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
