#include <sourcemeta/core/io.h>
#include <sourcemeta/core/test.h>

#include <barrier>   // std::barrier
#include <exception> // std::exception_ptr, std::current_exception
#include <mutex>     // std::mutex, std::scoped_lock
#include <thread>    // std::jthread

#if !defined(_WIN32)
#include <unistd.h> // pathconf, _PC_NAME_MAX
#endif

#include <array>        // std::array
#include <cstddef>      // std::byte
#include <filesystem>   // std::filesystem
#include <fstream>      // std::ifstream, std::istreambuf_iterator
#include <ios>          // std::ios::binary
#include <ostream>      // std::ostream
#include <span>         // std::span
#include <stdexcept>    // std::runtime_error
#include <string>       // std::string
#include <string_view>  // std::string_view
#include <system_error> // std::error_code

class IOAtomicWriteFileTest {
protected:
  IOAtomicWriteFileTest() {
    std::filesystem::create_directories(this->workspace_);
  }

  ~IOAtomicWriteFileTest() {
    std::error_code error;
    std::filesystem::remove_all(this->workspace_, error);
  }

  [[nodiscard]] auto staging_count(const std::filesystem::path &path) const
      -> std::size_t {
    const auto prefix{path.filename().string() + ".tmp."};
    std::size_t count{0};
    for (const auto &entry :
         std::filesystem::directory_iterator{path.parent_path()}) {
      if (entry.path().filename().string().starts_with(prefix)) {
        ++count;
      }
    }
    return count;
  }

  auto expect_no_staging(const std::filesystem::path &path) const -> void {
    EXPECT_EQ(this->staging_count(path), 0uz);
  }

  // The tests are always sequential, so using the same path is safe
  std::filesystem::path workspace_{std::filesystem::path{BUILD_DIRECTORY} /
                                   "sourcemeta_core_io_atomic_write_test"};
};

TEST_F(IOAtomicWriteFileTest, string_view_creates_file) {
  const auto path{this->workspace_ / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "hello");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "hello");
}

TEST_F(IOAtomicWriteFileTest, empty_contents) {
  const auto path{this->workspace_ / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(std::filesystem::file_size(path), 0);
}

TEST_F(IOAtomicWriteFileTest, byte_span_creates_file) {
  const auto path{this->workspace_ / "out.bin"};
  constexpr std::array<std::byte, 5> BYTES{{std::byte{0x48}, std::byte{0x65},
                                            std::byte{0x6c}, std::byte{0x6c},
                                            std::byte{0x6f}}};
  sourcemeta::core::atomic_write_file(path, std::span<const std::byte>{BYTES});
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "Hello");
}

TEST_F(IOAtomicWriteFileTest, callback_variant) {
  const auto path{this->workspace_ / "out.txt"};
  sourcemeta::core::atomic_write_file(path, [](std::ostream &stream) -> void {
    stream << "first";
    stream << " ";
    stream << "second";
  });
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "first second");
}

TEST_F(IOAtomicWriteFileTest, creates_parent_directories) {
  const auto path{this->workspace_ / "a" / "b" / "c" / "deep.txt"};
  sourcemeta::core::atomic_write_file(path, "nested");
  EXPECT_TRUE(std::filesystem::exists(path));
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "nested");
}

TEST_F(IOAtomicWriteFileTest, overwrites_existing_file) {
  const auto path{this->workspace_ / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "original");
  sourcemeta::core::atomic_write_file(path, "replacement");
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "replacement");
}

TEST_F(IOAtomicWriteFileTest,
       exception_in_callback_leaves_destination_untouched) {
  const auto path{this->workspace_ / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "original");

  try {
    sourcemeta::core::atomic_write_file(path,
                                        [&path](std::ostream &stream) -> void {
                                          stream << "partial replacement";
                                          if (!path.empty()) {
                                            throw std::runtime_error{"boom"};
                                          }
                                        });
    FAIL();
  } catch (const std::runtime_error &) {
    EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "original");
  } catch (...) {
    FAIL();
  }
}

TEST_F(IOAtomicWriteFileTest, exception_in_callback_removes_staging_file) {
  const auto path{this->workspace_ / "out.txt"};

  try {
    sourcemeta::core::atomic_write_file(
        path, [this, &path](std::ostream &stream) -> void {
          EXPECT_EQ(this->staging_count(path), 1uz);
          stream << "partial replacement";
          if (!path.empty()) {
            throw std::runtime_error{"boom"};
          }
        });
    FAIL();
  } catch (const std::runtime_error &) {
    this->expect_no_staging(path);
    EXPECT_FALSE(std::filesystem::exists(path));
  } catch (...) {
    FAIL();
  }
}

TEST_F(IOAtomicWriteFileTest, no_leftover_staging_on_success) {
  const auto path{this->workspace_ / "out.txt"};

  sourcemeta::core::atomic_write_file(path, "done");

  EXPECT_TRUE(std::filesystem::exists(path));
  this->expect_no_staging(path);
}

TEST_F(IOAtomicWriteFileTest, binary_writes_preserve_exact_bytes) {
  const auto path{this->workspace_ / "out.txt"};
  sourcemeta::core::atomic_write_file(path, "line one\nline two\n");

  std::ifstream stream{path, std::ios::binary};
  std::string actual;
  actual.assign(std::istreambuf_iterator<char>{stream},
                std::istreambuf_iterator<char>{});
  EXPECT_EQ(actual, "line one\nline two\n");
}

// POSIX permission bits don't map cleanly to Windows ACLs, and the
// EACCES-via-errno detection inside `atomic_write_file` is POSIX-only.
#if !defined(_WIN32)
TEST_F(IOAtomicWriteFileTest, read_only_parent_throws_permission_error) {
  const auto read_only_dir{this->workspace_ / "locked"};
  std::filesystem::create_directory(read_only_dir);
  std::filesystem::permissions(read_only_dir,
                               std::filesystem::perms::owner_read |
                                   std::filesystem::perms::owner_exec,
                               std::filesystem::perm_options::replace);
  const auto path{read_only_dir / "out.txt"};

  try {
    sourcemeta::core::atomic_write_file(path, "should fail");
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &error) {
    EXPECT_EQ(error.path(), path);
  } catch (...) {
    FAIL();
  }
  std::filesystem::permissions(read_only_dir, std::filesystem::perms::owner_all,
                               std::filesystem::perm_options::add);
}
#endif

// Staging the replacement needs a directory to stage it in, and a parent that
// is a file of its own refuses the open for a reason that is not about
// permissions
TEST_F(IOAtomicWriteFileTest, parent_that_is_a_file_of_its_own) {
  const auto blocker{this->workspace_ / "blocker"};
  sourcemeta::core::write_file(blocker, "x");
  const auto path{blocker / "out.txt"};
  try {
    sourcemeta::core::atomic_write_file(path, "hello");
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &) {
    FAIL();
  } catch (const std::exception &error) {
    EXPECT_FALSE(std::string_view{error.what()}.empty());
  }
}

TEST_F(IOAtomicWriteFileTest,
       failed_commit_preserves_directory_and_removes_staging) {
  const auto path{this->workspace_ / "out.txt"};
  std::filesystem::create_directory(path);
  sourcemeta::core::write_file(path / "keep", "original");
  try {
    sourcemeta::core::atomic_write_file(path, "replacement");
    FAIL();
  } catch (const std::filesystem::filesystem_error &error) {
    EXPECT_EQ(error.path2(), path);
    EXPECT_TRUE(static_cast<bool>(error.code()));
  }
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path / "keep"), "original");
  this->expect_no_staging(path);
}

TEST_F(IOAtomicWriteFileTest,
       failed_stream_preserves_file_and_removes_staging) {
  const auto path{this->workspace_ / "out.txt"};
  sourcemeta::core::write_file(path, "original");
  try {
    sourcemeta::core::atomic_write_file(path, [](std::ostream &stream) {
      stream << "partial replacement";
      stream.setstate(std::ios::badbit);
    });
    FAIL();
  } catch (const std::ios_base::failure &) {
  }
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "original");
  this->expect_no_staging(path);
}

TEST_F(IOAtomicWriteFileTest, embedded_null_bytes_are_preserved) {
  const auto path{this->workspace_ / "out.txt"};
  const std::string contents{"a\0b", 3};
  sourcemeta::core::atomic_write_file(path, std::string_view{contents});
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), contents);
  this->expect_no_staging(path);
}

#if !defined(_WIN32)
// The destination name fits, but adding the randomized staging suffix does not.
// This reaches a non-permission open failure without racing another writer.
TEST_F(IOAtomicWriteFileTest, staging_name_exceeding_filesystem_limit) {
  const auto limit{pathconf(this->workspace_.c_str(), _PC_NAME_MAX)};
  EXPECT_GT(limit, 0L);
  if (limit <= 0) {
    return;
  }
  const auto path{this->workspace_ /
                  std::string(static_cast<std::size_t>(limit), 'x')};
  sourcemeta::core::write_file(path, "original");
  try {
    sourcemeta::core::atomic_write_file(path, "replacement");
    FAIL();
  } catch (const sourcemeta::core::IOFilePermissionError &) {
    FAIL();
  } catch (const std::ios_base::failure &) {
  }
  EXPECT_EQ(sourcemeta::core::read_file_to_string(path), "original");
  this->expect_no_staging(path);
}
#endif

TEST_F(IOAtomicWriteFileTest, concurrent_writers_publish_one_complete_value) {
  const auto path{this->workspace_ / "out.txt"};
  const std::array<std::string, 2> contents{
      {std::string(8192, 'a'), std::string(8192, 'b')}};
  std::barrier ready{2};
  std::exception_ptr error;
  std::mutex error_mutex;
  auto write = [&](const std::size_t index) {
    bool entered{false};
    try {
      sourcemeta::core::atomic_write_file(path, [&](std::ostream &stream) {
        entered = true;
        ready.arrive_and_wait();
        stream << contents[index];
      });
    } catch (...) {
      if (!entered) {
        ready.arrive_and_drop();
      }
      std::scoped_lock lock{error_mutex};
      error = std::current_exception();
    }
  };
  {
    std::jthread first{write, 0uz};
    std::jthread second{write, 1uz};
  }
  EXPECT_FALSE(static_cast<bool>(error));
  const auto actual{sourcemeta::core::read_file_to_string(path)};
  EXPECT_TRUE(actual == contents[0] || actual == contents[1]);
  this->expect_no_staging(path);
}

TEST_F(IOAtomicWriteFileTest, bare_filename_uses_current_directory) {
  struct CurrentDirectory {
    std::filesystem::path previous{std::filesystem::current_path()};
    ~CurrentDirectory() {
      std::error_code error;
      std::filesystem::current_path(previous, error);
    }
  } restore;
  std::filesystem::current_path(this->workspace_);
  sourcemeta::core::atomic_write_file("out.txt", "relative");
  EXPECT_EQ(sourcemeta::core::read_file_to_string("out.txt"), "relative");
  for (const auto &entry : std::filesystem::directory_iterator{"."}) {
    EXPECT_EQ(entry.path().filename(), "out.txt");
  }
}
