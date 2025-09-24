#include <sourcemeta/core/gzip.h>

#include <fstream>  // std::ofstream
#include <iostream> // std::cerr

auto main(int argc, char *argv[]) -> int {
  if (argc < 4 || (argc % 2) != 0) {
    std::cerr << "Usage: " << argv[0]
              << " <output.tgz> <file1> <content1> [file2] [content2] ...\n";
    return 1;
  }

  sourcemeta::core::GZIPTar archive;

  for (int index = 2; index < argc; index += 2) {
    archive.push(argv[index], argv[index + 1]);
  }

  std::ofstream file{argv[1], std::ios::binary};
  file.write(archive.data(), static_cast<std::streamsize>(archive.size()));

  return 0;
}
