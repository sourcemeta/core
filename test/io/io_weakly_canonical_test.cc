#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>

TEST(IO_weakly_canonical, test_txt) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{STUBS_DIRECTORY} / "test.txt")};
  EXPECT_EQ(path, std::filesystem::path{STUBS_DIRECTORY} / "test.txt");
}

TEST(IO_weakly_canonical, not_exists) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{STUBS_DIRECTORY} / "foo.txt")};
  EXPECT_EQ(path, std::filesystem::path{STUBS_DIRECTORY} / "foo.txt");
}

TEST(IO_weakly_canonical, empty) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{}),
            std::filesystem::path{});
}

#ifndef _WIN32

TEST(IO_weakly_canonical, posix_trailing_slash) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{"/tmp/"}),
            sourcemeta::core::weakly_canonical(std::filesystem::path{"/tmp"}));
}

TEST(IO_weakly_canonical, posix_dot_segment_middle) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/./bar"}),
      std::filesystem::path{"/foo/bar"});
}

TEST(IO_weakly_canonical, posix_dot_segment_end) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/bar/."}),
      std::filesystem::path{"/foo/bar"});
}

TEST(IO_weakly_canonical, posix_dotdot_cancels_preceding) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/bar/../baz"}),
            std::filesystem::path{"/foo/baz"});
}

TEST(IO_weakly_canonical, posix_dotdot_chain_clamps_at_root) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/bar/../../../etc"}),
            std::filesystem::path{"/etc"});
}

TEST(IO_weakly_canonical, posix_dotdot_at_start_clamps_at_root) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/../../foo"}),
      std::filesystem::path{"/foo"});
}

TEST(IO_weakly_canonical, posix_double_slash) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"//foo//bar//baz"}),
            std::filesystem::path{"/foo/bar/baz"});
}

TEST(IO_weakly_canonical, posix_only_slashes) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{"////"}),
            std::filesystem::path{"/"});
}

TEST(IO_weakly_canonical, posix_dot_only) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{"/."}),
            std::filesystem::path{"/"});
}

TEST(IO_weakly_canonical, posix_trailing_dotdot_cancels_last) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/bar/.."}),
      std::filesystem::path{"/foo"});
}

TEST(IO_weakly_canonical, posix_mixed_dot_and_dotdot) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/./bar/../baz/."}),
            std::filesystem::path{"/foo/baz"});
}

TEST(IO_weakly_canonical, posix_traversal_through_existing_root) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/../etc/passwd"}),
            std::filesystem::path{"/etc/passwd"});
}

TEST(IO_weakly_canonical, posix_no_change_for_clean_path) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/bar/baz"}),
      std::filesystem::path{"/foo/bar/baz"});
}

#endif
