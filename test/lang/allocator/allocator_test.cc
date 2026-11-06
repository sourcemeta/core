#include <sourcemeta/core/allocator.h>
#include <sourcemeta/core/test.h>

#ifdef SOURCEMETA_CORE_TEST_ALLOCATOR_MIMALLOC
#include <mimalloc.h>

#include <cstddef> // std::size_t
#include <cstdlib> // std::malloc, std::calloc, std::realloc, std::free, posix_memalign, aligned_alloc
#include <cstring> // std::memset, strdup
#include <new>     // std::align_val_t

#if defined(__APPLE__)
#include <malloc/malloc.h> // malloc_size, malloc_good_size
#else
#include <malloc.h> // malloc_usable_size
#endif

static constexpr std::size_t REQUEST_SIZE{64};
static constexpr std::size_t REQUEST_ALIGNMENT{64};

// Freshly handed out storage holds nothing in particular, and a compiler that
// sees it reach a function taking a pointer to constant memory has to assume
// that function reads it. Writing to the block first settles that, and confirms
// the request was honoured for its full length while we are here
static auto landed_in_this_allocator(void *pointer, const std::size_t size)
    -> bool {
  std::memset(pointer, 0, size);
  return mi_is_in_heap_region(pointer);
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

TEST(Allocator_strdup_comes_from_this_allocator) {
  char *copy{::strdup("sourcemeta")};
  EXPECT_STREQ(copy, "sourcemeta");
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

TEST(Allocator_aligned_new_comes_from_this_allocator) {
  void *pointer{
      ::operator new(REQUEST_SIZE, std::align_val_t{REQUEST_ALIGNMENT})};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete(pointer, std::align_val_t{REQUEST_ALIGNMENT});
}

TEST(Allocator_nothrow_new_comes_from_this_allocator) {
  void *pointer{::operator new(REQUEST_SIZE, std::nothrow)};
  EXPECT_TRUE(landed_in_this_allocator(pointer, REQUEST_SIZE));
  ::operator delete(pointer, std::nothrow);
}

#if defined(__APPLE__)

TEST(Allocator_malloc_size_covers_the_request) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  std::memset(pointer, 0, REQUEST_SIZE);
  EXPECT_GE(::malloc_size(pointer), REQUEST_SIZE);
  std::free(pointer);
}

TEST(Allocator_malloc_good_size_covers_the_request) {
  EXPECT_GE(::malloc_good_size(REQUEST_SIZE), REQUEST_SIZE);
}

#else

TEST(Allocator_malloc_usable_size_covers_the_request) {
  void *pointer{std::malloc(REQUEST_SIZE)};
  std::memset(pointer, 0, REQUEST_SIZE);
  EXPECT_GE(::malloc_usable_size(pointer), REQUEST_SIZE);
  std::free(pointer);
}

#endif

#endif
