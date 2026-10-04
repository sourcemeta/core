#include <sourcemeta/core/mcp.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <new>
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
        mcp_make_resource("https://example.com/" + std::to_string(index),
                          "resource-" + std::to_string(index),
                          "application/json", std::string(256, 'x')));
  }
  page.assign("resources", std::move(resources));
  measure(page, false);
  measure(page, true);
  std::array<std::int64_t, 5> copy_times{};
  std::array<std::int64_t, 5> borrowed_times{};
  Measurement copy{};
  Measurement borrowed{};
  for (std::size_t index = 0; index < copy_times.size(); ++index) {
    copy = measure(page, false);
    borrowed = measure(page, true);
    copy_times[index] = copy.microseconds;
    borrowed_times[index] = borrowed.microseconds;
  }
  if (copy.output_bytes != borrowed.output_bytes) {
    return 1;
  }
  std::sort(copy_times.begin(), copy_times.end());
  std::sort(borrowed_times.begin(), borrowed_times.end());
  std::cout << "mode,resources,new_calls,new_bytes,output_bytes,median_us\n"
            << "copy,4096," << copy.allocations << ',' << copy.allocated_bytes
            << ',' << copy.output_bytes << ',' << copy_times[2] << '\n'
            << "borrowed,4096," << borrowed.allocations << ','
            << borrowed.allocated_bytes << ',' << borrowed.output_bytes << ','
            << borrowed_times[2] << '\n';
}
