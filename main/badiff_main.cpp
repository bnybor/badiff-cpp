#include <fstream>
#include <string>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <badiff/badiff.hpp>
#include <badiff/q/op_queue.hpp>

static void help() {
  printf(
      "badiff diff <original> <target> <delta>         Create a delta file\n");
  printf(
      "badiff patch <original> <delta> <target>        Apply a delta file\n");
}

int main(int argc, const char **argv) {
  if (argc < 2) {
    help();
    return -1;
  }

  mallopt(M_MMAP_THRESHOLD, 128 * 1024 * 1024);

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

    struct stat original_stat;
    int fd;
    fd = open(argv[2], O_RDONLY, 0);
    fstat(fd, &original_stat);
    close(fd);

    struct stat target_stat;
    fd = open(argv[3], O_RDONLY, 0);
    fstat(fd, &target_stat);
    close(fd);

    auto diff = badiff::Diff::Make(original, original_stat.st_size, target,
                                   target_stat.st_size);

    delta.write(diff->diff.get(), diff->len);

    printf("\n");

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
