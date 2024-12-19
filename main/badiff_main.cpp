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
  printf("badiff diff [<optimization>] <original> <target> <delta>\n"
         "      Create a delta from original to target.\n"
         "      <optimization>      Level of delta-size optimization\n"
         "            -O0           Very fast, very low "
         "optimization\n"
         "            -O1           Fast, low optimization\n"
         "            -O2           Normal.\n"
         "            -O3           Slow, high optimization\n"
         "            -O4           Very slow, very high "
         "optimization\n"
         "      <original>          Original file\n"
         "      <target>            Target file\n"
         "      <delta>             Delta file\n"
         "\n"
         "badiff apply <original> <delta> <target>\n"
         "      Apply a delta file from original to target.\n"
         "      <original>          Original file\n"
         "      <target>            Target file\n"
         "      <delta>             Delta file\n");
}

int main(int argc, const char **argv) {
  using badiff::Diff;

  if (argc < 2) {
    help();
    return EXIT_FAILURE;
  }

  badiff::CONSOLE_OUTPUT = true;

  mallopt(M_MMAP_THRESHOLD, 128 * 1024 * 1024);

  const char **arg = argv;
  std::string command(*++arg);

  if (command == "help") {
    help();
    return EXIT_SUCCESS;
  } else if (command == "diff") {
    if (argc != 5 && argc != 6) {
      help();
      return EXIT_FAILURE;
    }

    int chunk_size = Diff::NORMAL_CHUNK;
    std::string opt(*++arg);
    if (opt.size() == 3 && std::string(opt.begin(), opt.begin() + 2) == "-O") {
      if (opt.at(3) == '0')
        chunk_size = Diff::VERY_FAST_CHUNK;
      else if (opt.at(3) == '1')
        chunk_size = Diff::FAST_CHUNK;
      else if (opt.at(3) == '3')
        chunk_size = Diff::NORMAL_CHUNK;
      else if (opt.at(3) == '4')
        chunk_size = Diff::EFFICIENT_CHUNK;
      else if (opt.at(3) == '5')
        chunk_size = Diff::VERY_EFFICIENT_CHUNK;
      else {
        help();
        return EXIT_FAILURE;
      }
    } else {
      --arg;
    }

    std::ifstream original(*++arg);
    std::ifstream target(*++arg);
    std::ofstream delta(*++arg);

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
                                   target_stat.st_size, chunk_size);

    delta.write(diff->diff.get(), diff->len);

    printf("\n");

  } else if (command == "apply") {
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
