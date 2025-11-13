// Comprehensive vocabulary benchmark - Real-world scenarios
// Tests edge cases and typical usage patterns from blaze compilation

#include "src/core/jsonschema/include/sourcemeta/core/jsonschema_vocabularies.h"
#include "src/core/json/include/sourcemeta/core/json.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <vector>
#include <random>

using namespace sourcemeta::core;

template <typename Func>
double benchmark(const std::string& name, Func func, int iterations = 100000) {
    auto start = std::chrono::high_resolution_clock::now();
    func(iterations);
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << std::setw(55) << std::left << name
              << std::setw(10) << std::right << std::fixed << std::setprecision(2)
              << ms << " ms"
              << std::setw(12) << std::right << (ms / iterations * 1000000) << " ns/op"
              << std::endl;
    return ms;
}

void scenario_schema_compilation() {
    std::cout << "\n=== SCENARIO 1: Schema Compilation (Blaze typical usage) ===" << std::endl;
    std::cout << "Simulates vocabulary lookups during JSON Schema compilation" << std::endl;

    // Typical blaze schema compilation involves many vocabulary checks
    // for keywords to determine their behavior
    const std::vector<std::string> keyword_vocabs = {
        "https://json-schema.org/draft/2020-12/vocab/applicator",
        "https://json-schema.org/draft/2020-12/vocab/validation",
        "https://json-schema.org/draft/2020-12/vocab/core",
        "https://json-schema.org/draft/2020-12/vocab/unevaluated",
        "https://json-schema.org/draft/2020-12/vocab/applicator", // repeated
        "https://json-schema.org/draft/2020-12/vocab/validation",  // repeated
        "https://json-schema.org/draft/2020-12/vocab/core",        // repeated
    };

    benchmark("  Vocabulary lookups per schema (10k schemas)", [&](int iterations) {
        Vocabularies vocabs;
        vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/core", true});
        vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/applicator", true});
        vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/validation", true});
        vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/unevaluated", true});

        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            for (const auto& vocab : keyword_vocabs) {
                result = vocabs.contains(vocab);
            }
        }
    }, 10000);
}

void scenario_known_vs_custom() {
    std::cout << "\n=== SCENARIO 2: Known vs Custom Vocabulary Performance ===" << std::endl;
    std::cout << "Comparing performance for known (bitset) vs custom (hashmap) vocabularies" << std::endl;

    Vocabularies vocabs;
    // Mix of known and custom
    vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/core", true});
    vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/applicator", true});
    vocabs.insert({"https://example.com/custom1", true});
    vocabs.insert({"https://example.com/custom2", true});

    benchmark("  Known vocabulary lookup (bitset path)", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            result = vocabs.contains("https://json-schema.org/draft/2020-12/vocab/core");
        }
    });

    benchmark("  Custom vocabulary lookup (hashmap path)", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            result = vocabs.contains("https://example.com/custom1");
        }
    });

    benchmark("  Non-existent vocabulary lookup", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            result = vocabs.contains("https://non-existent.com/vocab");
        }
    });
}

void scenario_all_28_known() {
    std::cout << "\n=== SCENARIO 3: All 28 Known Vocabularies ===" << std::endl;
    std::cout << "Performance with maximum known vocabulary count" << std::endl;

    Vocabularies vocabs;
    // Add all 28 known vocabularies
    const std::vector<std::pair<std::string, bool>> all_known = {
        {"http://json-schema.org/draft-00/schema#", true},
        {"http://json-schema.org/draft-00/hyper-schema#", true},
        {"http://json-schema.org/draft-01/schema#", true},
        {"http://json-schema.org/draft-01/hyper-schema#", true},
        {"http://json-schema.org/draft-02/schema#", true},
        {"http://json-schema.org/draft-02/hyper-schema#", true},
        {"http://json-schema.org/draft-03/schema#", true},
        {"http://json-schema.org/draft-03/hyper-schema#", true},
        {"http://json-schema.org/draft-04/schema#", true},
        {"http://json-schema.org/draft-04/hyper-schema#", true},
        {"http://json-schema.org/draft-06/schema#", true},
        {"http://json-schema.org/draft-06/hyper-schema#", true},
        {"http://json-schema.org/draft-07/schema#", true},
        {"http://json-schema.org/draft-07/hyper-schema#", true},
        {"https://json-schema.org/draft/2019-09/vocab/core", true},
        {"https://json-schema.org/draft/2019-09/vocab/applicator", true},
        {"https://json-schema.org/draft/2019-09/vocab/validation", true},
        {"https://json-schema.org/draft/2019-09/vocab/meta-data", true},
        {"https://json-schema.org/draft/2019-09/vocab/format", true},
        {"https://json-schema.org/draft/2019-09/vocab/content", true},
        {"https://json-schema.org/draft/2019-09/vocab/hyper-schema", true},
        {"https://json-schema.org/draft/2020-12/vocab/core", true},
        {"https://json-schema.org/draft/2020-12/vocab/applicator", true},
        {"https://json-schema.org/draft/2020-12/vocab/unevaluated", true},
        {"https://json-schema.org/draft/2020-12/vocab/validation", true},
        {"https://json-schema.org/draft/2020-12/vocab/meta-data", true},
        {"https://json-schema.org/draft/2020-12/vocab/format-annotation", true},
        {"https://json-schema.org/draft/2020-12/vocab/format-assertion", true},
        {"https://json-schema.org/draft/2020-12/vocab/content", true}
    };

    for (const auto& entry : all_known) {
        vocabs.insert(entry);
    }

    std::cout << "  Inserted 28 known vocabularies, bitset size: " << sizeof(uint32_t) * 2 << " bytes" << std::endl;

    // Random access pattern
    std::mt19937 rng(42);
    std::uniform_int_distribution<size_t> dist(0, all_known.size() - 1);

    benchmark("  Random vocabulary lookup (28 vocabs)", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            size_t idx = dist(rng);
            result = vocabs.contains(all_known[idx].first);
        }
    });

    benchmark("  all_vocabularies() call", [&](int iterations) {
        for (int i = 0; i < iterations; i++) {
            auto all = vocabs.all_vocabularies();
            volatile size_t size = all.size();
        }
    }, 10000);
}

void scenario_disabled_vocabularies() {
    std::cout << "\n=== SCENARIO 4: Disabled Vocabularies (false values) ===" << std::endl;
    std::cout << "Performance with explicitly disabled vocabularies" << std::endl;

    Vocabularies vocabs;
    vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/core", true});
    vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/applicator", true});
    vocabs.insert({"https://json-schema.org/draft/2019-09/vocab/format", false});  // disabled
    vocabs.insert({"https://json-schema.org/draft/2020-12/vocab/format-assertion", false});  // disabled

    benchmark("  Lookup enabled vocabulary", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            result = vocabs.contains("https://json-schema.org/draft/2020-12/vocab/core");
        }
    });

    benchmark("  Lookup disabled vocabulary", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            result = vocabs.contains("https://json-schema.org/draft/2019-09/vocab/format");
        }
    });

    benchmark("  find() on disabled vocabulary", [&](int iterations) {
        volatile bool result = false;
        for (int i = 0; i < iterations; i++) {
            auto status = vocabs.find("https://json-schema.org/draft/2019-09/vocab/format");
            if (status.has_value()) {
                result = status.value();
            }
        }
    });
}

void scenario_merge_heavy() {
    std::cout << "\n=== SCENARIO 5: Merge Operations (Schema inheritance) ===" << std::endl;
    std::cout << "Simulates vocabulary merging during schema composition" << std::endl;

    // Base metaschema vocabularies
    Vocabularies base;
    base.insert({"https://json-schema.org/draft/2020-12/vocab/core", true});
    base.insert({"https://json-schema.org/draft/2020-12/vocab/applicator", true});
    base.insert({"https://json-schema.org/draft/2020-12/vocab/validation", true});

    // Extension vocabularies
    Vocabularies extension1;
    extension1.insert({"https://json-schema.org/draft/2020-12/vocab/meta-data", true});
    extension1.insert({"https://example.com/extension/v1", true});

    Vocabularies extension2;
    extension2.insert({"https://json-schema.org/draft/2020-12/vocab/unevaluated", true});
    extension2.insert({"https://example.com/extension/v2", true});

    benchmark("  Single merge operation", [&](int iterations) {
        for (int i = 0; i < iterations; i++) {
            Vocabularies temp = base;
            temp.merge(extension1);
        }
    }, 50000);

    benchmark("  Chained merge operations", [&](int iterations) {
        for (int i = 0; i < iterations; i++) {
            Vocabularies temp = base;
            temp.merge(extension1);
            temp.merge(extension2);
        }
    }, 50000);

    benchmark("  Merge with conflicts (no-op)", [&](int iterations) {
        for (int i = 0; i < iterations; i++) {
            Vocabularies temp = base;
            temp.merge(base);  // All keys already exist
        }
    }, 50000);
}

void memory_footprint_analysis() {
    std::cout << "\n=== MEMORY FOOTPRINT ANALYSIS ===" << std::endl;

    std::cout << "  sizeof(Vocabularies):           " << sizeof(Vocabularies) << " bytes" << std::endl;
    std::cout << "  sizeof(uint32_t) * 2:           " << sizeof(uint32_t) * 2 << " bytes (bitflags)" << std::endl;
    std::cout << "  sizeof(std::unordered_map<>):   ~" << sizeof(std::unordered_map<std::string, bool>) << " bytes (empty)" << std::endl;

    // Estimate memory for different scenarios
    std::cout << "\n  Estimated memory usage:" << std::endl;
    std::cout << "    - 0 custom vocabs:  " << sizeof(Vocabularies) << " bytes" << std::endl;
    std::cout << "    - 5 custom vocabs:  ~" << (sizeof(Vocabularies) + 5 * 64) << " bytes" << std::endl;
    std::cout << "    - 10 custom vocabs: ~" << (sizeof(Vocabularies) + 10 * 64) << " bytes" << std::endl;

    std::cout << "\n  Baseline unordered_map estimate:" << std::endl;
    std::cout << "    - 7 vocabs:  ~" << (sizeof(std::unordered_map<std::string, bool>) + 7 * 96) << " bytes" << std::endl;
    std::cout << "    - 28 vocabs: ~" << (sizeof(std::unordered_map<std::string, bool>) + 28 * 96) << " bytes" << std::endl;

    double savings = (1.0 - (double)sizeof(Vocabularies) / (sizeof(std::unordered_map<std::string, bool>) + 7 * 96)) * 100.0;
    std::cout << "\n  Memory savings for typical case (7 vocabs): " << std::fixed << std::setprecision(1) << savings << "%" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  DETAILED VOCABULARY BENCHMARK" << std::endl;
    std::cout << "========================================" << std::endl;

    scenario_schema_compilation();
    scenario_known_vs_custom();
    scenario_all_28_known();
    scenario_disabled_vocabularies();
    scenario_merge_heavy();
    memory_footprint_analysis();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  ALL BENCHMARKS COMPLETE" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
