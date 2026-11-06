// This project stays within ISO C, under which the platform declares only what
// the standard asks of it, while most of the entry points below are extensions
// that it hides until a build says it wants them
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include <mimalloc.h>

#include <stdlib.h> // size_t, malloc, calloc, realloc, free, posix_memalign, aligned_alloc, valloc, reallocarray
#include <string.h> // strdup, strndup

#if defined(__APPLE__)
#include <AvailabilityMacros.h> // MAC_OS_X_VERSION_10_7, MAC_OS_X_VERSION_MAX_ALLOWED
#include <malloc/malloc.h>      // malloc_size, malloc_good_size
#else
#include <malloc.h> // memalign, pvalloc, malloc_usable_size
#endif

// This library hides its symbols by default, which these definitions cannot
// afford, as the whole point of them is that a program already calls them by
// name. Link time optimisation has to keep them for the same reason, even
// though nothing here refers to them
#define SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT                                   \
  __attribute__((visibility("default"), used))

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *malloc(size_t size) { return mi_malloc(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *calloc(size_t count, size_t size) { return mi_calloc(count, size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *realloc(void *pointer, size_t size) { return mi_realloc(pointer, size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void free(void *pointer) { mi_free(pointer); }

// Neither of the two below has to go through this allocator, but on some
// platforms they reach for something more primitive than the general entry
// point, and on others the platform makes them macros that cannot be replaced
#if !defined(strdup)
SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
char *strdup(const char *string) { return mi_strdup(string); }
#endif

#if !defined(strndup) &&                                                       \
    (!defined(__APPLE__) ||                                                    \
     (defined(MAC_OS_X_VERSION_10_7) &&                                        \
      MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7))
SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
char *strndup(const char *string, size_t length) {
  return mi_strndup(string, length);
}
#endif

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *reallocf(void *pointer, size_t size) {
  return mi_reallocf(pointer, size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
size_t malloc_size(const void *pointer) { return mi_malloc_size(pointer); }

#if !defined(__ANDROID__)
SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
size_t malloc_usable_size(void *pointer) {
  return mi_malloc_usable_size(pointer);
}
#else
SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
size_t malloc_usable_size(const void *pointer) {
  return mi_malloc_usable_size(pointer);
}
#endif

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *valloc(size_t size) { return mi_valloc(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void vfree(void *pointer) { mi_free(pointer); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
size_t malloc_good_size(size_t size) { return mi_malloc_good_size(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
int posix_memalign(void **pointer, size_t alignment, size_t size) {
  return mi_posix_memalign(pointer, alignment, size);
}

// A certain platform declares this one as an inline function of its own, which
// no definition here can replace. That inline function allocates through one of
// the entry points above, so it ends up here anyway
#if !defined(__GLIBC__) || __USE_ISOC11
SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *aligned_alloc(size_t alignment, size_t size) {
  return mi_aligned_alloc(alignment, size);
}
#endif

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void cfree(void *pointer) { mi_free(pointer); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *pvalloc(size_t size) { return mi_pvalloc(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *memalign(size_t alignment, size_t size) {
  return mi_memalign(alignment, size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *_aligned_malloc(size_t size, size_t alignment) {
  return mi_malloc_aligned(size, alignment);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *reallocarray(void *pointer, size_t count, size_t size) {
  return mi_reallocarray(pointer, count, size);
}

// Some platforms ship one of their own, so defer to theirs where it exists
SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
__attribute__((weak)) int reallocarr(void *pointer, size_t count, size_t size) {
  return mi_reallocarr(pointer, count, size);
}

// The platform's own general purpose allocator reaches its implementation
// through the names below rather than through the entry points above, and
// parts of it keep doing so no matter what those entry points say
#if defined(__linux__)

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *__libc_malloc(size_t size) { return mi_malloc(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *__libc_calloc(size_t count, size_t size) {
  return mi_calloc(count, size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *__libc_realloc(void *pointer, size_t size) {
  return mi_realloc(pointer, size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void __libc_free(void *pointer) { mi_free(pointer); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void __libc_cfree(void *pointer) { mi_free(pointer); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *__libc_valloc(size_t size) { return mi_valloc(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *__libc_pvalloc(size_t size) { return mi_pvalloc(size); }

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
void *__libc_memalign(size_t alignment, size_t size) {
  return mi_memalign(alignment, size);
}

SOURCEMETA_CORE_ALLOCATOR_ENTRYPOINT
int __posix_memalign(void **pointer, size_t alignment, size_t size) {
  return mi_posix_memalign(pointer, alignment, size);
}

#endif
