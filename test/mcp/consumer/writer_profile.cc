#include <sourcemeta/core/mcp.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <new>
#include <sstream>
#include <streambuf>

namespace {
std::size_t allocations = 0;
std::size_t allocated_bytes = 0;
bool measuring = false;

class CountingBuffer : public std::streambuf {
public:
  std::size_t bytes = 0;

protected:
  auto xsputn(const char *, const std::streamsize count)
      -> std::streamsize override {
    bytes += static_cast<std::size_t>(count);
    return count;
  }
  auto overflow(const int character) -> int override {
    if (character != traits_type::eof()) {
      ++bytes;
    }
    return traits_type::not_eof(character);
  }
};

struct Measurement {
  std::size_t allocations;
  std::size_t allocated_bytes;
  std::size_t output_bytes;
  std::int64_t microseconds;
};

auto measure(const sourcemeta::core::JSON &page, const bool borrowed)
    -> Measurement {
  using namespace sourcemeta::core;
  CountingBuffer buffer;
  std::ostream stream{&buffer};
  allocations = 0;
  allocated_bytes = 0;
  const auto start{std::chrono::steady_clock::now()};
  measuring = true;
  if (borrowed) {
    mcp_write_result(stream, MCPProtocolVersion::V_2026_07_28, "resources/list",
                     JSON{0}, page, MCPCachePolicy{});
  } else {
    auto result{mcp_decorate_cacheable_result(
        MCPProtocolVersion::V_2026_07_28, JSON{page}, {}, "resources/list")};
    const auto envelope{jsonrpc_make_success(JSON{0}, std::move(result))};
    stringify(envelope, stream);
  }
  measuring = false;
  const auto elapsed{std::chrono::duration_cast<std::chrono::microseconds>(
                         std::chrono::steady_clock::now() - start)
                         .count()};
  return {allocations, allocated_bytes, buffer.bytes, elapsed};
}
} // namespace

// This standalone consumer deliberately uses the system allocator. Its global
// replacements count C++ new requests, not RSS or allocator-internal activity.
auto operator new(const std::size_t size) -> void * {
  if (void *pointer = std::malloc(size == 0 ? 1 : size)) {
    if (measuring) {
      ++allocations;
      allocated_bytes += size;
    }
    return pointer;
  }
  throw std::bad_alloc{};
}
void operator delete(void *pointer) noexcept { std::free(pointer); }
void operator delete(void *pointer, std::size_t) noexcept {
  std::free(pointer);
}

int main() {
  using namespace sourcemeta::core;
  auto page{JSON::make_object()};
  auto resources{JSON::make_array()};
  for (std::size_t index = 0; index < 4096; ++index) {
    resources.push_back(
        mcp_make_resource(sourcemeta::core::MCPProtocolVersion::V_2025_03_26,
                          "https://example.com/" + std::to_string(index),
                          "resource-" + std::to_string(index),
                          "application/json", std::string(256, 'x')));
  }
  page.assign("resources", std::move(resources));

  // Check the entire output outside the measured region; byte counts alone
  // cannot establish that the streaming path emitted the correct contents.
  std::ostringstream reference;
  stringify(jsonrpc_make_success(JSON{0}, mcp_decorate_cacheable_result(
                                              MCPProtocolVersion::V_2026_07_28,
                                              page, {}, "resources/list")),
            reference);
  std::ostringstream streamed;
  mcp_write_result(streamed, MCPProtocolVersion::V_2026_07_28, "resources/list",
                   JSON{0}, page, MCPCachePolicy{});
  if (parse_json(reference.str()) != parse_json(streamed.str())) {
    return 1;
  }
  // Warm both orders, then alternate the first route to limit ordering bias.
  measure(page, false);
  measure(page, true);
  measure(page, true);
  measure(page, false);
  std::array<Measurement, 7> copies{};
  std::array<Measurement, 7> borrowed{};
  for (std::size_t index = 0; index < copies.size(); ++index) {
    if (index % 2 == 0) {
      copies[index] = measure(page, false);
      borrowed[index] = measure(page, true);
    } else {
      borrowed[index] = measure(page, true);
      copies[index] = measure(page, false);
    }
    if (copies[index].output_bytes != borrowed[index].output_bytes ||
        borrowed[index].allocations >= copies[index].allocations ||
        borrowed[index].allocated_bytes >= copies[index].allocated_bytes) {
      return 1;
    }
  }
  std::cout << "mode,sample,resources,new_calls,new_bytes,output_bytes,"
               "microseconds\n";
  const auto report = [](const char *mode, const auto &samples) {
    std::array<std::size_t, 7> calls{};
    std::array<std::size_t, 7> bytes{};
    std::array<std::int64_t, 7> times{};
    for (std::size_t index = 0; index < samples.size(); ++index) {
      const auto &sample{samples[index]};
      calls[index] = sample.allocations;
      bytes[index] = sample.allocated_bytes;
      times[index] = sample.microseconds;
      std::cout << mode << ',' << index << ",4096," << sample.allocations << ','
                << sample.allocated_bytes << ',' << sample.output_bytes << ','
                << sample.microseconds << '\n';
    }
    std::sort(calls.begin(), calls.end());
    std::sort(bytes.begin(), bytes.end());
    std::sort(times.begin(), times.end());
    std::cout << mode << ",median,4096," << calls[3] << ',' << bytes[3] << ','
              << samples.front().output_bytes << ',' << times[3] << '\n';
  };
  report("copy", copies);
  report("borrowed", borrowed);
}
