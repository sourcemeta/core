#include <sourcemeta/core/benchmark.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/options.h>
#include <sourcemeta/core/process.h>

#include <algorithm>   // std::max, std::min
#include <array>       // std::array
#include <cmath>       // std::llround
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <cstdlib>     // EXIT_SUCCESS, EXIT_FAILURE
#include <filesystem>  // std::filesystem::path
#include <fstream>     // std::ofstream
#include <functional>  // std::function
#include <iomanip>     // std::setprecision, std::fixed
#include <iostream>    // std::cout, std::cerr
#include <sstream>     // std::ostringstream
#include <string>      // std::string
#include <string_view> // std::string_view
#include <thread>      // std::thread::hardware_concurrency
#include <utility>     // std::move
#include <vector>      // std::vector

namespace {
using namespace std::string_view_literals;

constexpr auto BENCHMARK_HASH_NUM_CPUS{
    sourcemeta::core::JSON::Object::hash("num_cpus"sv)};
constexpr auto BENCHMARK_HASH_NAME{
    sourcemeta::core::JSON::Object::hash("name"sv)};
constexpr auto BENCHMARK_HASH_FAMILY_INDEX{
    sourcemeta::core::JSON::Object::hash("family_index"sv)};
constexpr auto BENCHMARK_HASH_PER_FAMILY_INSTANCE_INDEX{
    sourcemeta::core::JSON::Object::hash("per_family_instance_index"sv)};
constexpr auto BENCHMARK_HASH_RUN_NAME{
    sourcemeta::core::JSON::Object::hash("run_name"sv)};
constexpr auto BENCHMARK_HASH_RUN_TYPE{
    sourcemeta::core::JSON::Object::hash("run_type"sv)};
constexpr auto BENCHMARK_HASH_REPETITIONS{
    sourcemeta::core::JSON::Object::hash("repetitions"sv)};
constexpr auto BENCHMARK_HASH_REPETITION_INDEX{
    sourcemeta::core::JSON::Object::hash("repetition_index"sv)};
constexpr auto BENCHMARK_HASH_THREADS{
    sourcemeta::core::JSON::Object::hash("threads"sv)};
constexpr auto BENCHMARK_HASH_ITERATIONS{
    sourcemeta::core::JSON::Object::hash("iterations"sv)};
constexpr auto BENCHMARK_HASH_REAL_TIME{
    sourcemeta::core::JSON::Object::hash("real_time"sv)};
constexpr auto BENCHMARK_HASH_CPU_TIME{
    sourcemeta::core::JSON::Object::hash("cpu_time"sv)};
constexpr auto BENCHMARK_HASH_TIME_UNIT{
    sourcemeta::core::JSON::Object::hash("time_unit"sv)};
constexpr auto BENCHMARK_HASH_CONTEXT{
    sourcemeta::core::JSON::Object::hash("context"sv)};
constexpr auto BENCHMARK_HASH_BENCHMARKS{
    sourcemeta::core::JSON::Object::hash("benchmarks"sv)};

struct RegisteredBenchmark {
  std::string name;
  std::string_view file;
  int line;
  std::function<void(sourcemeta::core::BenchmarkState &)> body;
};

struct Measurement {
  std::string name;
  std::uint64_t iterations;
  double real_time;
  std::optional<double> cpu_time;
  // What the very first execution cost, before anything the benchmark touches
  // had a chance to warm up. A wide gap against the reported time is the sign
  // of a cache, a lazy initialisation, or an allocator that settles
  double cold_time;
};

auto registry() -> std::vector<RegisteredBenchmark> & {
  static std::vector<RegisteredBenchmark> benchmarks;
  return benchmarks;
}

// How long a single benchmark is given before its result is taken as
// representative. Matches the convention the previous framework established, so
// that recorded numbers stay comparable across the change
constexpr std::chrono::nanoseconds MINIMUM_RUN_TIME{500'000'000};

// A run that cannot reach the target time within this many iterations is
// reported as it stands rather than grown further
constexpr std::uint64_t MAXIMUM_ITERATIONS{1'000'000'000'000};

constexpr std::uint64_t INITIAL_ITERATIONS{1};

// Aim beyond the target rather than exactly at it, so that a run which lands
// just short does not need another attempt
constexpr double OVERSHOOT{1.4};

// A run too short to extrapolate from grows by a flat factor instead, as
// scaling from a measurement dominated by noise overshoots wildly
constexpr double BLIND_GROWTH{10.0};
constexpr double BLIND_THRESHOLD{0.1};

auto next_iterations(const std::uint64_t current,
                     const std::chrono::nanoseconds elapsed) -> std::uint64_t {
  const auto target{static_cast<double>(MINIMUM_RUN_TIME.count())};
  const auto measured{std::max(static_cast<double>(elapsed.count()), 1.0)};
  const auto multiplier{(measured / target) > BLIND_THRESHOLD
                            ? (target * OVERSHOOT / measured)
                            : BLIND_GROWTH};
  const auto grown{
      std::llround(std::max(multiplier * static_cast<double>(current),
                            static_cast<double>(current) + 1.0))};
  return std::min(static_cast<std::uint64_t>(grown), MAXIMUM_ITERATIONS);
}

auto measure(const RegisteredBenchmark &benchmark)
    -> std::optional<Measurement> {
  auto iterations{INITIAL_ITERATIONS};
  std::optional<double> cold{std::nullopt};

  while (true) {
    sourcemeta::core::BenchmarkState state{iterations};
    benchmark.body(state);

    if (!state.measured()) {
      return std::nullopt;
    }

    const auto elapsed{state.real_time()};
    if (!cold.has_value()) {
      cold = static_cast<double>(elapsed.count()) /
             static_cast<double>(iterations);
    }

    if (elapsed >= MINIMUM_RUN_TIME || iterations >= MAXIMUM_ITERATIONS) {
      const auto count{static_cast<double>(iterations)};
      std::optional<double> cpu{std::nullopt};
      if (state.cpu_time().has_value()) {
        cpu = static_cast<double>(state.cpu_time().value().count()) / count;
      }

      return Measurement{.name = benchmark.name,
                         .iterations = iterations,
                         .real_time =
                             static_cast<double>(elapsed.count()) / count,
                         .cpu_time = cpu,
                         .cold_time = cold.value()};
    }

    iterations = next_iterations(iterations, elapsed);
  }
}

auto print_usage(const std::string_view program) -> void {
  std::cout << "Usage: " << std::filesystem::path{program}.stem().string()
            << " [options]\n\n"
            << "Run the registered benchmarks.\n\n"
            << "Options:\n"
            << "  -f, --filter <text>  Only run benchmarks whose name contains "
               "<text>\n"
            << "  -o, --output <path>  Also write the results as JSON to "
               "<path>\n"
            << "  -h, --help           Show this message\n";
}

auto print_environment() -> void {
  const auto cores{std::thread::hardware_concurrency()};
  if (cores > 0) {
    std::cout << "Running on " << cores << " processors\n";
  }
}

// A benchmark that takes seconds per iteration and one that takes nanoseconds
// both have to line up in the same table, so the unit follows the magnitude
auto format_duration(const double nanoseconds) -> std::string {
  struct Unit {
    double factor;
    std::string_view suffix;
  };

  static constexpr std::array<Unit, 4> UNITS{
      {{.factor = 1.0, .suffix = "ns"},
       {.factor = 1'000.0, .suffix = "us"},
       {.factor = 1'000'000.0, .suffix = "ms"},
       {.factor = 1'000'000'000.0, .suffix = "s"}}};

  auto chosen{UNITS.front()};
  for (const auto &unit : UNITS) {
    if (nanoseconds >= unit.factor) {
      chosen = unit;
    }
  }

  std::ostringstream stream;
  stream << std::fixed << std::setprecision(2) << (nanoseconds / chosen.factor)
         << " " << chosen.suffix;
  return stream.str();
}

auto print_measurement(const Measurement &measurement) -> void {
  std::cout << measurement.name << "\n"
            << "  time: " << format_duration(measurement.real_time);
  if (measurement.cpu_time.has_value()) {
    std::cout << ", cpu: " << format_duration(measurement.cpu_time.value());
  }

  std::cout << ", first: " << format_duration(measurement.cold_time)
            << ", iterations: " << measurement.iterations << "\n";
}

auto to_json(const std::vector<Measurement> &measurements)
    -> sourcemeta::core::JSON {
  auto context{sourcemeta::core::JSON::make_object()};
  const auto cores{std::thread::hardware_concurrency()};
  if (cores > 0) {
    context.assign_assume_new(
        "num_cpus", sourcemeta::core::JSON{static_cast<std::int64_t>(cores)},
        BENCHMARK_HASH_NUM_CPUS);
  }

  auto entries{sourcemeta::core::JSON::make_array()};
  std::size_t index{0};
  for (const auto &measurement : measurements) {
    auto entry{sourcemeta::core::JSON::make_object()};
    entry.assign_assume_new("name", sourcemeta::core::JSON{measurement.name},
                            BENCHMARK_HASH_NAME);
    entry.assign_assume_new(
        "family_index",
        sourcemeta::core::JSON{static_cast<std::int64_t>(index)},
        BENCHMARK_HASH_FAMILY_INDEX);
    entry.assign_assume_new("per_family_instance_index",
                            sourcemeta::core::JSON{0},
                            BENCHMARK_HASH_PER_FAMILY_INSTANCE_INDEX);
    entry.assign_assume_new("run_name",
                            sourcemeta::core::JSON{measurement.name},
                            BENCHMARK_HASH_RUN_NAME);
    entry.assign_assume_new("run_type", sourcemeta::core::JSON{"iteration"},
                            BENCHMARK_HASH_RUN_TYPE);
    entry.assign_assume_new("repetitions", sourcemeta::core::JSON{1},
                            BENCHMARK_HASH_REPETITIONS);
    entry.assign_assume_new("repetition_index", sourcemeta::core::JSON{0},
                            BENCHMARK_HASH_REPETITION_INDEX);
    entry.assign_assume_new("threads", sourcemeta::core::JSON{1},
                            BENCHMARK_HASH_THREADS);
    entry.assign_assume_new("iterations",
                            sourcemeta::core::JSON{static_cast<std::int64_t>(
                                measurement.iterations)},
                            BENCHMARK_HASH_ITERATIONS);
    entry.assign_assume_new("real_time",
                            sourcemeta::core::JSON{measurement.real_time},
                            BENCHMARK_HASH_REAL_TIME);
    entry.assign_assume_new(
        "cpu_time",
        sourcemeta::core::JSON{
            measurement.cpu_time.value_or(measurement.real_time)},
        BENCHMARK_HASH_CPU_TIME);
    entry.assign_assume_new("time_unit", sourcemeta::core::JSON{"ns"},
                            BENCHMARK_HASH_TIME_UNIT);
    entries.push_back(std::move(entry));
    index += 1;
  }

  auto document{sourcemeta::core::JSON::make_object()};
  document.assign_assume_new("context", std::move(context),
                             BENCHMARK_HASH_CONTEXT);
  document.assign_assume_new("benchmarks", std::move(entries),
                             BENCHMARK_HASH_BENCHMARKS);
  return document;
}

} // namespace

namespace sourcemeta::core {

BenchmarkState::BenchmarkState(const std::uint64_t iterations)
    : iterations_{iterations} {}

auto BenchmarkState::iterations() const -> std::uint64_t {
  return this->iterations_;
}

auto BenchmarkState::measured() const -> bool { return this->finished_; }

auto BenchmarkState::real_time() const -> std::chrono::nanoseconds {
  return this->real_elapsed_;
}

auto BenchmarkState::cpu_time() const
    -> std::optional<std::chrono::nanoseconds> {
  return this->cpu_elapsed_;
}

auto BenchmarkState::start() -> void {
  this->started_ = true;
  this->cpu_start_ = process_usage().cpu_time;
  this->real_start_ = std::chrono::steady_clock::now();
}

auto BenchmarkState::finish() -> void {
  const auto real_end{std::chrono::steady_clock::now()};
  const auto cpu_end{process_usage().cpu_time};
  this->real_elapsed_ = real_end - this->real_start_;
  if (this->cpu_start_.has_value() && cpu_end.has_value()) {
    this->cpu_elapsed_ = cpu_end.value() - this->cpu_start_.value();
  }

  this->finished_ = true;
}

auto benchmark_use_char_pointer(const volatile char *) -> void {}

auto benchmark_register(std::string_view name, std::string_view file, int line,
                        std::function<void(BenchmarkState &)> body) -> int {
  registry().push_back({.name = std::string{name},
                        .file = file,
                        .line = line,
                        .body = std::move(body)});
  return 0;
}

auto benchmark_run(int argc, char **argv) -> int {
  Options options;
  options.option("filter", {"f"});
  options.option("output", {"o"});
  options.flag("help", {"h"});

  try {
    options.parse(argc, argv);
  } catch (const OptionsError &) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (options.contains("help")) {
    print_usage(argv[0]);
    return EXIT_SUCCESS;
  }

  std::string_view needle;
  if (options.contains("filter") && !options.at("filter").empty()) {
    needle = options.at("filter").front();
  }

  std::vector<const RegisteredBenchmark *> selected;
  for (const auto &entry : registry()) {
    if (needle.empty() || entry.name.contains(needle)) {
      selected.push_back(&entry);
    }
  }

  // An unfiltered run that selects nothing means registration is broken, which
  // must not pass as a successful run. A filter that matches nothing is instead
  // an ordinary thing to ask for interactively
  if (selected.empty() && needle.empty()) {
    std::cerr << "error: no benchmarks were registered\n";
    return EXIT_FAILURE;
  }

  print_environment();

  std::vector<Measurement> measurements;
  measurements.reserve(selected.size());
  for (const auto *entry : selected) {
    const auto measurement{measure(*entry)};
    if (!measurement.has_value()) {
      std::cerr << "error: benchmark did not iterate over its state: "
                << entry->name << "\n";
      return EXIT_FAILURE;
    }

    print_measurement(measurement.value());
    measurements.push_back(measurement.value());
  }

  if (options.contains("output") && !options.at("output").empty()) {
    const std::filesystem::path destination{options.at("output").front()};
    std::ofstream stream{destination};
    if (!stream) {
      std::cerr << "error: could not write to " << destination.string() << "\n";
      return EXIT_FAILURE;
    }

    prettify(to_json(measurements), stream);
    stream << "\n";
  }

  return EXIT_SUCCESS;
}

} // namespace sourcemeta::core
