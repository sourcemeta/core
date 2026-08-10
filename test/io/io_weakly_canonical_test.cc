#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <filesystem> // std::filesystem

#if !defined(_WIN32)
#include <sys/stat.h> // mkfifo, S_IRUSR, S_IWUSR
#endif

TEST(test_txt) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{STUBS_DIRECTORY} / "test.txt")};
  EXPECT_EQ(path, std::filesystem::path{STUBS_DIRECTORY} / "test.txt");
}

TEST(not_exists) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{STUBS_DIRECTORY} / "foo.txt")};
  EXPECT_EQ(path, std::filesystem::path{STUBS_DIRECTORY} / "foo.txt");
}

TEST(empty) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{}),
            std::filesystem::path{});
}

TEST(relative_that_does_not_exist) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{"sourcemeta-core-does-not-exist.txt"})};
  EXPECT_TRUE(path.is_absolute());
  EXPECT_EQ(path,
            std::filesystem::current_path() /
                std::filesystem::path{"sourcemeta-core-does-not-exist.txt"});
}

TEST(relative_with_missing_leading_component) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{"sourcemeta-core-nope"} /
      std::filesystem::path{"also-nope.txt"})};
  EXPECT_TRUE(path.is_absolute());
  EXPECT_EQ(path, std::filesystem::current_path() /
                      std::filesystem::path{"sourcemeta-core-nope"} /
                      std::filesystem::path{"also-nope.txt"});
}

TEST(relative_with_dot_segments_that_does_not_exist) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{"."} /
      std::filesystem::path{"sourcemeta-core-nope"} /
      std::filesystem::path{".."} /
      std::filesystem::path{"sourcemeta-core-also-nope.txt"})};
  EXPECT_TRUE(path.is_absolute());
  EXPECT_EQ(path, std::filesystem::current_path() /
                      std::filesystem::path{"sourcemeta-core-also-nope.txt"});
}

TEST(relative_that_exists) {
  const auto path{sourcemeta::core::weakly_canonical(
      std::filesystem::path{"."} / std::filesystem::path{"."})};
  EXPECT_TRUE(path.is_absolute());
  EXPECT_EQ(path, std::filesystem::current_path());
}

#ifndef _WIN32

TEST(posix_trailing_slash) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{"/tmp/"}),
            sourcemeta::core::weakly_canonical(std::filesystem::path{"/tmp"}));
}

TEST(posix_dot_segment_middle) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/./bar"}),
      std::filesystem::path{"/foo/bar"});
}

TEST(posix_dot_segment_end) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/bar/."}),
      std::filesystem::path{"/foo/bar"});
}

TEST(posix_dotdot_cancels_preceding) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/bar/../baz"}),
            std::filesystem::path{"/foo/baz"});
}

TEST(posix_dotdot_chain_clamps_at_root) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/bar/../../../etc"}),
            std::filesystem::path{"/etc"});
}

TEST(posix_dotdot_at_start_clamps_at_root) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/../../foo"}),
      std::filesystem::path{"/foo"});
}

TEST(posix_double_slash) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"//foo//bar//baz"}),
            std::filesystem::path{"/foo/bar/baz"});
}

TEST(posix_only_slashes) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{"////"}),
            std::filesystem::path{"/"});
}

TEST(posix_dot_only) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(std::filesystem::path{"/."}),
            std::filesystem::path{"/"});
}

TEST(posix_trailing_dotdot_cancels_last) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/bar/.."}),
      std::filesystem::path{"/foo"});
}

TEST(posix_mixed_dot_and_dotdot) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/./bar/../baz/."}),
            std::filesystem::path{"/foo/baz"});
}

TEST(posix_traversal_through_existing_root) {
  EXPECT_EQ(sourcemeta::core::weakly_canonical(
                std::filesystem::path{"/foo/../etc/passwd"}),
            std::filesystem::path{"/etc/passwd"});
}

TEST(posix_no_change_for_clean_path) {
  EXPECT_EQ(
      sourcemeta::core::weakly_canonical(std::filesystem::path{"/foo/bar/baz"}),
      std::filesystem::path{"/foo/bar/baz"});
}

TEST(posix_relative_fifo) {
  const auto fifo_path{std::filesystem::path{BUILD_DIRECTORY} /
                       "sourcemeta_core_io_weakly_canonical_fifo"};
  std::filesystem::remove(fifo_path);
  EXPECT_EQ(::mkfifo(fifo_path.c_str(), S_IRUSR | S_IWUSR), 0);
  const auto relative_fifo_path{std::filesystem::relative(fifo_path)};
  const auto path{sourcemeta::core::weakly_canonical(relative_fifo_path)};
  const auto expected{sourcemeta::core::weakly_canonical(fifo_path)};
  std::filesystem::remove(fifo_path);

  EXPECT_TRUE(relative_fifo_path.is_relative());
  EXPECT_TRUE(path.is_absolute());
  EXPECT_EQ(path, expected);
}

#endif
