#include <fstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <badiff/badiff.hpp>

static void help() {}

int main(int argc, const char **argv) {
  if (argc < 2) {
    help();
    return -1;
  }

  std::string command(argv[1]);

  if (command == "help") {
    help();
    return 0;
  } else if (command == "diff") {
    if (argc != 5) {
      help();
      return -1;
    }

    std::ifstream original(argv[2]);
    std::ifstream target(argv[3]);
    std::ofstream delta(argv[4]);

    auto diff = badiff::Diff::Make(original, target);

    delta.write(diff->diff.get(), diff->len);

  } else if (command == "patch") {
    if (argc != 5) {
      help();
      return -1;
    }
    std::string original(argv[2]);
    std::string diff(argv[3]);
    std::string target(argv[4]);
  } else {
    help();
    return -1;
  }

  return 0;
}
