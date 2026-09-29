# Benchmark: Allocator Evaluation (mimalloc vs. System Allocator)

This benchmark suite evaluates representative allocation-heavy workloads in Sourcemeta Core when built with **mimalloc** versus the **system allocator**.

This measurement framework is strictly observation-only: it establishes baseline empirical data to guide decisions about whether an allocator exploration is justified. It does not implement a custom allocator or change allocator defaults.

## Representative Workloads

Rather than synthetic allocation loops (`malloc` / `free` in isolation), which fail to reflect real data structures, cache localities, and memory lifetimes, the benchmarks evaluate real Core operations:

1. **JSON Parsing** (`JSON_Parse_Schema_ISO_Language`, `JSON_Parse_KrakenD`):
   - Ingests large real-world documents (`2020_12_iso_language_2023_set_3.json`, 1.1 MB; `2019_09_krakend.json`, 556 KB).
   - Stresses dynamic allocation for strings, vector containers, and polymorphic JSON variant nodes.
   - File reading is performed outside the timed region.

2. **JSON Serialization** (`JSON_Stringify_ISO_Language`, `JSON_Prettify_KrakenD`):
   - Serializes parsed JSON documents to compact and pretty formats into string streams.
   - Measures stream buffer growth, continuous string reallocation, and AST traversal.
   - Parsing is performed outside the timed region.

3. **JSON Schema Compilation and Bundling** (`Schema_Frame_KrakenD_References`, `Schema_Frame_ISO_Language_Locations`, `Schema_Bundle_Many_Remotes_With_Fragments`):
   - Evaluates schema location extraction, reference indexing, and remote fragment resolution.
   - Creates and populates hash maps, location vectors, and subschema reference graphs.

4. **JSONPath Evaluation** (`JSONPath_Descendant_Filter_Nested`):
   - Evaluates descendant wildcard queries and filter expressions on nested JSON objects.

## Platform Support

In `CMakeLists.txt`, mimalloc is conditionally selected on **Linux** and **macOS** when `SOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR=OFF` and sanitizers are disabled:
- **Linux & macOS**: Support both mimalloc and the system allocator.
- **Windows**: Defaults to the system allocator.

## Reproducible A/B Comparison Procedure

To compare allocators accurately, maintainers must build separate, clean configurations using identical compiler flags, source revisions, and optimization levels (`Release`).

### 1. Build A: System Allocator

```bash
# Configure with system allocator
cmake -S . -B build-system \
  -DCMAKE_BUILD_TYPE=Release \
  -DSOURCEMETA_CORE_BENCHMARK=ON \
  -DSOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR=ON

# Compile the benchmark binary
cmake --build build-system --config Release --parallel

# Execute benchmarks and produce JSON report
cmake --build build-system --config Release --target benchmark
```

The results are written to `./build-system/benchmark.json`.

### 2. Build B: mimalloc

```bash
# Configure with mimalloc (default on Linux/macOS)
cmake -S . -B build-mimalloc \
  -DCMAKE_BUILD_TYPE=Release \
  -DSOURCEMETA_CORE_BENCHMARK=ON \
  -DSOURCEMETA_CORE_USE_SYSTEM_ALLOCATOR=OFF

# Compile the benchmark binary
cmake --build build-mimalloc --config Release --parallel

# Execute benchmarks and produce JSON report
cmake --build build-mimalloc --config Release --target benchmark
```

The results are written to `./build-mimalloc/benchmark.json`.

### 3. Comparing Results

Use the repository's comparison tool to analyze throughput deltas:

```bash
python3 ./contrib/benchmark-compare.py \
  "system-allocator" \
  ./build-system/benchmark.json \
  ./build-mimalloc/benchmark.json
```

This generates a comparative Markdown report highlighting speedups or regressions across all workloads.

To focus specifically on the allocation-sensitive workloads, use the filter flag:

```bash
./build-system/benchmark/sourcemeta_core_benchmarks --filter JSON_Parse_KrakenD
./build-mimalloc/benchmark/sourcemeta_core_benchmarks --filter JSON_Parse_KrakenD
```

### 4. Peak Memory (RSS) Measurement

Memory profiling should be conducted **independently** from timing runs to avoid instrumentation overhead:

- **Linux**:
  ```bash
  /usr/bin/time -v ./build-system/benchmark/sourcemeta_core_benchmarks --filter JSON_Parse_KrakenD
  /usr/bin/time -v ./build-mimalloc/benchmark/sourcemeta_core_benchmarks --filter JSON_Parse_KrakenD
  ```
  Look for `Maximum resident set size (kbytes)`.

- **macOS**:
  ```bash
  /usr/bin/time -l ./build-system/benchmark/sourcemeta_core_benchmarks --filter JSON_Parse_KrakenD
  /usr/bin/time -l ./build-mimalloc/benchmark/sourcemeta_core_benchmarks --filter JSON_Parse_KrakenD
  ```
  Look for `maximum resident set size`.

- **Windows (PowerShell)**:
  ```powershell
  $p = Start-Process -FilePath ".\build-system\benchmark\sourcemeta_core_benchmarks.exe" -ArgumentList "--filter JSON_Parse_KrakenD" -NoNewWindow -PassThru -Wait
  $p.PeakWorkingSet64 / 1MB
  ```
