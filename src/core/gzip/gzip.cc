#include <sourcemeta/core/gzip.h>

extern "C" {
#include <libdeflate.h>
}

#include <algorithm> // std::min
#include <memory>    // std::unique_ptr

namespace sourcemeta::core {

auto gzip(const std::uint8_t *input, const std::size_t size, const int level)
    -> std::string {
  std::unique_ptr<libdeflate_compressor, decltype(&libdeflate_free_compressor)>
      compressor{libdeflate_alloc_compressor(level),
                 libdeflate_free_compressor};
  if (!compressor) {
    throw GZIPError{"Could not allocate compressor"};
  }

  const auto max_size{libdeflate_gzip_compress_bound(compressor.get(), size)};
  std::string output;
  std::size_t actual_size{0};
  // libdeflate overwrites the whole bound, so leaving the buffer uninitialised
  // avoids zero-filling multi-megabyte allocations that are immediately
  // discarded
  output.resize_and_overwrite(
      max_size,
      [&](char *const buffer, const std::size_t capacity) -> std::size_t {
        actual_size = libdeflate_gzip_compress(compressor.get(), input, size,
                                               buffer, capacity);
        return capacity;
      });

  if (actual_size == 0) {
    throw GZIPError{"Could not compress input"};
  }

  output.resize(actual_size);
  return output;
}

auto gunzip(const std::uint8_t *input, const std::size_t size,
            const std::size_t output_hint, const std::size_t maximum_size)
    -> std::string {
  std::unique_ptr<libdeflate_decompressor,
                  decltype(&libdeflate_free_decompressor)>
      decompressor{libdeflate_alloc_decompressor(),
                   libdeflate_free_decompressor};
  if (!decompressor) {
    throw GZIPError{"Could not allocate decompressor"};
  }

  std::string output;
  // Bound the very first allocation too, so a hint or the size heuristic cannot
  // exceed the cap before the loop has a chance to reject the input. The size
  // heuristic is guarded so that the multiplication cannot overflow before the
  // clamp, since the cap is caller-controlled
  std::size_t capacity{0};
  if (output_hint > 0) {
    capacity = std::min(output_hint, maximum_size);
  } else if (size > maximum_size / 4) {
    capacity = maximum_size;
  } else {
    capacity = size * 4;
  }

  for (;;) {
    std::size_t actual_size{0};
    auto result{LIBDEFLATE_BAD_DATA};
    // libdeflate writes only the decompressed bytes, so leaving the buffer
    // uninitialised avoids zero-filling multi-megabyte allocations on every
    // retry of the doubling loop
    output.resize_and_overwrite(
        capacity,
        [&](char *const buffer, const std::size_t buffer_size) -> std::size_t {
          result =
              libdeflate_gzip_decompress(decompressor.get(), input, size,
                                         buffer, buffer_size, &actual_size);
          return buffer_size;
        });

    if (result == LIBDEFLATE_SUCCESS) {
      output.resize(actual_size);
      return output;
    }

    if (result == LIBDEFLATE_INSUFFICIENT_SPACE) {
      if (capacity >= maximum_size) {
        throw GZIPError{"Decompressed output exceeds the maximum allowed size"};
      }

      // Double without overflowing: capacity is below the cap here, so doubling
      // only runs when the result still fits under it
      capacity = (capacity > maximum_size / 2) ? maximum_size : capacity * 2;
      continue;
    }

    throw GZIPError{"Could not decompress input"};
  }
}

} // namespace sourcemeta::core
