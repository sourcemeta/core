#include <mimalloc.h>

#include <cstddef> // std::size_t
#include <new>     // std::align_val_t, std::nothrow_t

// This library hides its symbols by default, which these definitions cannot
// afford, as the whole point of them is that a program already calls them by
// name. Link time optimisation has to keep them for the same reason, even
// though nothing here refers to them
#define SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT                                   \
  __attribute__((visibility("default"), used))

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new(const std::size_t size) -> void * { return mi_new(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new[](const std::size_t size) -> void * { return mi_new(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new(const std::size_t size, const std::nothrow_t &) noexcept
    -> void * {
  return mi_new_nothrow(size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new[](const std::size_t size, const std::nothrow_t &) noexcept
    -> void * {
  return mi_new_nothrow(size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new(const std::size_t size, const std::align_val_t alignment)
    -> void * {
  return mi_new_aligned(size, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new[](const std::size_t size, const std::align_val_t alignment)
    -> void * {
  return mi_new_aligned(size, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new(const std::size_t size, const std::align_val_t alignment,
                  const std::nothrow_t &) noexcept -> void * {
  return mi_new_aligned_nothrow(size, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator new[](const std::size_t size, const std::align_val_t alignment,
                    const std::nothrow_t &) noexcept -> void * {
  return mi_new_aligned_nothrow(size, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete(void *pointer) noexcept -> void { mi_free(pointer); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete[](void *pointer) noexcept -> void { mi_free(pointer); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete(void *pointer, const std::nothrow_t &) noexcept -> void {
  mi_free(pointer);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete[](void *pointer, const std::nothrow_t &) noexcept -> void {
  mi_free(pointer);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete(void *pointer, const std::size_t size) noexcept -> void {
  mi_free_size(pointer, size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete[](void *pointer, const std::size_t size) noexcept -> void {
  mi_free_size(pointer, size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete(void *pointer, const std::align_val_t alignment) noexcept
    -> void {
  mi_free_aligned(pointer, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete[](void *pointer, const std::align_val_t alignment) noexcept
    -> void {
  mi_free_aligned(pointer, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete(void *pointer, const std::align_val_t alignment,
                     const std::nothrow_t &) noexcept -> void {
  mi_free_aligned(pointer, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete[](void *pointer, const std::align_val_t alignment,
                       const std::nothrow_t &) noexcept -> void {
  mi_free_aligned(pointer, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete(void *pointer, const std::size_t size,
                     const std::align_val_t alignment) noexcept -> void {
  mi_free_size_aligned(pointer, size, static_cast<std::size_t>(alignment));
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
auto operator delete[](void *pointer, const std::size_t size,
                       const std::align_val_t alignment) noexcept -> void {
  mi_free_size_aligned(pointer, size, static_cast<std::size_t>(alignment));
}
