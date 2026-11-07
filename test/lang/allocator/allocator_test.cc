#include <sourcemeta/core/allocator.h>
#include <sourcemeta/core/test.h>

#ifdef SOURCEMETA_CORE_TEST_ALLOCATOR_MIMALLOC
#include <mimalloc.h>

#include <cstddef> // std::size_t
#include <cstdio>  // asprintf
#include <cstdlib> // std::malloc, std::calloc, std::realloc, std::free, posix_memalign, aligned_alloc, valloc
#include <cstring> // std::memset, strdup, strndup
#include <limits>  // std::numeric_limits
#include <new>     // std::align_val_t, std::nothrow, std::bad_alloc

#if defined(__APPLE__)
#include <malloc/malloc.h> // malloc_size, malloc_good_size
#else
#include <malloc.h> // malloc_usable_size, memalign, pvalloc
#endif

static constexpr std::size_t REQUEST_SIZE{64};
static constexpr std::size_t REQUEST_ALIGNMENT{64};

// The largest request a well formed program is allowed to make, and far more
// than any machine can hand over, so that asking for it leaves nothing but the
// path that refuses. Asking for more is something the compiler itself objects
// to, which would leave nothing here to run
static constexpr std::size_t IMPOSSIBLE_REQUEST_SIZE{
    std::numeric_limits<std::ptrdiff_t>::max()};

// Freshly handed out storage holds nothing in particular, and a compiler that
// sees it reach a function taking a pointer to constant memory has to assume
// that function reads it. Writing to the block first settles that, and confirms
// the request was honoured for its full length while we are here
static auto landed_in_this_allocator(void *pointer, const std::size_t size)
    -> bool {
  if (pointer == nullptr) {
    return false;
  }

  std::memset(pointer, 0, size);
  return mi_is_in_heap_region(pointer);
}

// A platform declares the entry points it brings of its own, and this library
// replaces a few more that the platform it is built for does not declare,
// either because the name belongs to another platform or because it belongs to
// the inside of a platform's own library. Writing those down is the only way
// this suite can ask for them
extern "C" {

void vfree(void *pointer);
void cfree(void *pointer);
int reallocarr(void *pointer, std::size_t count, std::size_t size);

#if defined(__APPLE__)
void *pvalloc(std::size_t size);
void *memalign(std::size_t alignment, std::size_t size);
void *reallocarray(void *pointer, std::size_t count, std::size_t size);
std::size_t malloc_usable_size(void *pointer);
#else
void *reallocf(void *pointer, std::size_t size);
std::size_t malloc_size(const void *pointer);
std::size_t malloc_good_size(std::size_t size);
#endif

// The ones below answer to a shape of name that a platform keeps for itself,
// which is not a shape this project gets to choose for a name it did not pick
// NOLINTBEGIN(bugprone-reserved-identifier,cert-dcl51-cpp,readability-identifier-naming)
void *_aligned_malloc(std::size_t size, std::size_t alignment);

#if defined(__linux__)
void *__libc_malloc(std::size_t size);
void *__libc_calloc(std::size_t count, std::size_t size);
void *__libc_realloc(void *pointer, std::size_t size);
void __libc_free(void *pointer);
void __libc_cfree(void *pointer);
void *__libc_valloc(std::size_t size);
void *__libc_pvalloc(std::size_t size);
void *__libc_memalign(std::size_t alignment, std::size_t size);
int __posix_memalign(void **pointer, std::size_t alignment, std::size_t size);
#endif
// NOLINTEND(bugprone-reserved-identifier,cert-dcl51-cpp,readability-identifier-naming)
}
#endif

TEST(Allocator_is_stable_across_calls) {
  EXPECT_EQ(sourcemeta::core::allocator(), sourcemeta::core::allocator());
}

TEST(Allocator_is_the_one_this_build_selected) {
#ifdef SOURCEMETA_CORE_TEST_ALLOCATOR_MIMALLOC
  EXPECT_EQ(sourcemeta::core::allocator(),
            sourcemeta::core::Allocator::Mimalloc);
#else
  EXPECT_EQ(sourcemeta::core::allocator(), sourcemeta::core::Allocator::System);
#endif
}

#ifdef SOURCEMETA_CORE_TEST_ALLOCATOR_MIMALLOC

TEST(Allocator_does_not_claim_memory_it_did_not_hand_out) {
  int automatic{42};
  EXPECT_FALSE(mi_is_in_heap_region(&automatic));
}

TEST(Allocator_malloc_comes_from_this_allocator) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  std::free(pointer);
}

TEST(Allocator_calloc_comes_from_this_allocator) {
  void *pointer{std::calloc(2, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE * 2));
  std::free(pointer);
}

TEST(Allocator_realloc_comes_from_this_allocator) {
  void *pointer{std::realloc(nullptr, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  void *grown{std::realloc(pointer, REQUEST_SIZE * 4)};
  EXPECT_TRUE(landed_in_this_allocator(grown, REQUEST_SIZE * 4));
  std::free(grown);
}

TEST(Allocator_reallocf_comes_from_this_allocator) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  void *grown{::reallocf(pointer, REQUEST_SIZE * 4)};
  EXPECT_TRUE(landed_in_this_allocator(grown, REQUEST_SIZE * 4));
  std::free(grown);
}

TEST(Allocator_reallocarray_comes_from_this_allocator) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  void *grown{::reallocarray(pointer, 4, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(grown, REQUEST_SIZE * 4));
  std::free(grown);
}

TEST(Allocator_reallocarr_comes_from_this_allocator) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  EXPECT_EQ(::reallocarr(static_cast<void *>(&pointer), 4, REQUEST_SIZE), 0);
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE * 4));
  std::free(pointer);
}

TEST(Allocator_aligned_alloc_comes_from_this_allocator) {
  void *pointer{::aligned_alloc(REQUEST_ALIGNMENT, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  std::free(pointer);
}

TEST(Allocator_posix_memalign_comes_from_this_allocator) {
  void *pointer{nullptr};
  EXPECT_EQ(::posix_memalign(&pointer, REQUEST_ALIGNMENT, REQUEST_SIZE), 0);
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  std::free(pointer);
}

TEST(Allocator_memalign_comes_from_this_allocator) {
  void *pointer{::memalign(REQUEST_ALIGNMENT, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  std::free(pointer);
}

TEST(Allocator_aligned_malloc_comes_from_this_allocator) {
  void *pointer{::_aligned_malloc(REQUEST_SIZE, REQUEST_ALIGNMENT)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  std::free(pointer);
}

TEST(Allocator_valloc_comes_from_this_allocator) {
  void *pointer{::valloc(REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::vfree(pointer);
}

TEST(Allocator_pvalloc_comes_from_this_allocator) {
  void *pointer{::pvalloc(REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  std::free(pointer);
}

TEST(Allocator_cfree_takes_back_what_this_allocator_handed_out) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::cfree(pointer);
}

TEST(Allocator_strdup_comes_from_this_allocator) {
  char *copy{::strdup("sourcemeta")};
  EXPECT_STREQ(copy, "sourcemeta");
  EXPECT_TRUE(mi_is_in_heap_region(copy));
  std::free(copy);
}

TEST(Allocator_strndup_comes_from_this_allocator) {
  char *copy{::strndup("sourcemeta", 6)};
  EXPECT_STREQ(copy, "source");
  EXPECT_TRUE(mi_is_in_heap_region(copy));
  std::free(copy);
}

TEST(Allocator_new_comes_from_this_allocator) {
  auto *pointer{new int{42}};
  EXPECT_EQ(*pointer, 42);
  EXPECT_TRUE(mi_is_in_heap_region(pointer));
  delete pointer;
}

TEST(Allocator_new_array_comes_from_this_allocator) {
  auto *pointer{new int[REQUEST_SIZE]{}};
  EXPECT_TRUE(mi_is_in_heap_region(pointer));
  delete[] pointer;
}

TEST(Allocator_nothrow_new_comes_from_this_allocator) {
  void *pointer{::operator new(REQUEST_SIZE, std::nothrow)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete(pointer, std::nothrow);
}

TEST(Allocator_nothrow_new_array_comes_from_this_allocator) {
  void *pointer{::operator new[](REQUEST_SIZE, std::nothrow)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete[](pointer, std::nothrow);
}

TEST(Allocator_sized_array_delete_takes_back_what_it_handed_out) {
  void *pointer{::operator new[](REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete[](pointer, REQUEST_SIZE);
}

TEST(Allocator_aligned_new_comes_from_this_allocator) {
  void *pointer{
      ::operator new(REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT})};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete(pointer, std::align_val_t{REQUEST_ALIGNMENT});
}

TEST(Allocator_aligned_new_array_comes_from_this_allocator) {
  void *pointer{
      ::operator new[](REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT})};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete[](pointer, std::align_val_t{REQUEST_ALIGNMENT});
}

TEST(Allocator_aligned_nothrow_new_comes_from_this_allocator) {
  void *pointer{::operator new(
      REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT}, std::nothrow)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete(pointer, std::align_val_t{REQUEST_ALIGNMENT}, std::nothrow);
}

TEST(Allocator_aligned_nothrow_new_array_comes_from_this_allocator) {
  void *pointer{::operator new[](
      REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT}, std::nothrow)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete[](pointer, std::align_val_t{REQUEST_ALIGNMENT},
                      std::nothrow);
}

TEST(Allocator_sized_aligned_delete_takes_back_what_it_handed_out) {
  void *pointer{
      ::operator new(REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT})};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete(pointer, REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT});
}

TEST(Allocator_sized_aligned_array_delete_takes_back_what_it_handed_out) {
  void *pointer{
      ::operator new[](REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT})};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete[](pointer, REQUEST_SIZE,
                      std::align_val_t{REQUEST_ALIGNMENT});
}

TEST(Allocator_new_reports_a_request_it_cannot_satisfy) {
  void *pointer{nullptr};
  try {
    pointer = ::operator new(IMPOSSIBLE_REQUEST_SIZE);
    FAIL();
  } catch (const std::bad_alloc &) {
    // Refusing a request of this size is expected
  }

  EXPECT_EQ(pointer, nullptr);
}

TEST(Allocator_nothrow_new_answers_with_nothing_when_it_cannot_satisfy) {
  void *pointer{::operator new(IMPOSSIBLE_REQUEST_SIZE, std::nothrow)};
  EXPECT_EQ(pointer, nullptr);
}

TEST(Allocator_malloc_size_covers_the_request) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  std::memset(pointer, 0, REQUEST_SIZE);
  EXPECT_GE(::malloc_size(pointer), REQUEST_SIZE);
  std::free(pointer);
}

TEST(Allocator_malloc_usable_size_covers_the_request) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  std::memset(pointer, 0, REQUEST_SIZE);
  EXPECT_GE(::malloc_usable_size(pointer), REQUEST_SIZE);
  std::free(pointer);
}

TEST(Allocator_malloc_good_size_covers_the_request) {
  EXPECT_GE(::malloc_good_size(REQUEST_SIZE), REQUEST_SIZE);
}

#if defined(__linux__)

TEST(Allocator_libc_malloc_comes_from_this_allocator) {
  void *pointer{::__libc_malloc(REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::__libc_free(pointer);
}

TEST(Allocator_libc_calloc_comes_from_this_allocator) {
  void *pointer{::__libc_calloc(2, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE * 2));
  ::__libc_cfree(pointer);
}

TEST(Allocator_libc_realloc_comes_from_this_allocator) {
  void *pointer{::__libc_realloc(nullptr, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  void *grown{::__libc_realloc(pointer, REQUEST_SIZE * 4)};
  EXPECT_TRUE(landed_in_this_allocator(grown, REQUEST_SIZE * 4));
  ::__libc_free(grown);
}

TEST(Allocator_libc_valloc_comes_from_this_allocator) {
  void *pointer{::__libc_valloc(REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::__libc_free(pointer);
}

TEST(Allocator_libc_pvalloc_comes_from_this_allocator) {
  void *pointer{::__libc_pvalloc(REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::__libc_free(pointer);
}

TEST(Allocator_libc_memalign_comes_from_this_allocator) {
  void *pointer{::__libc_memalign(REQUEST_ALIGNMENT, REQUEST_SIZE)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::__libc_free(pointer);
}

TEST(Allocator_libc_posix_memalign_comes_from_this_allocator) {
  void *pointer{nullptr};
  EXPECT_EQ(::__posix_memalign(&pointer, REQUEST_ALIGNMENT, REQUEST_SIZE), 0);
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::__libc_free(pointer);
}

#endif

// The platform's own library allocates on a program's behalf and hands the
// block over for the program to release, so a block it allocated has to be one
// of ours too. Where it is not, every such release is a leak, as this allocator
// declines to release a block it never handed out
TEST(Allocator_owns_what_the_platform_library_allocates) {
  char *text{nullptr};
  EXPECT_GT(::asprintf(&text, "%d", 42), 0);
  EXPECT_STREQ(text, "42");
  EXPECT_TRUE(mi_is_in_heap_region(text));
  std::free(text);
}

#endif
