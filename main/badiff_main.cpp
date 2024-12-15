#include <fstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <badiff/badiff.hpp>
#include <badiff/q/op_queue.hpp>

static void help() {
  printf(
      "badiff delta <original> <target> <delta>         Create a delta file\n");
  printf(
      "badiff patch <original> <delta> <target>        Apply a delta file\n");
}

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
    std::ifstream original(argv[2]);
    std::ifstream delta(argv[3]);
    std::ofstream target(argv[4]);

    badiff::q::OpQueue op_queue;
    op_queue.Deserialize(delta);
    op_queue.Apply(original, target);

  } else {
    help();
    return -1;
  }

  return 0;
}
