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
      if (opt.at(2) == '0')
        chunk_size = Diff::VERY_FAST_CHUNK;
      else if (opt.at(2) == '1')
        chunk_size = Diff::FAST_CHUNK;
      else if (opt.at(2) == '2')
        chunk_size = Diff::NORMAL_CHUNK;
      else if (opt.at(2) == '3')
        chunk_size = Diff::EFFICIENT_CHUNK;
      else if (opt.at(2) == '4')
        chunk_size = Diff::VERY_EFFICIENT_CHUNK;
      else {
        help();
        return EXIT_FAILURE;
      }
      ++argv;
    } else {
      --arg;
    }

    std::string original(*++arg);
    std::string target(*++arg);
    std::string delta(*++arg);

    std::ifstream original_stream(original);
    std::ifstream target_stream(target);

    struct stat original_stat;
    int fd;
    fd = open(original.c_str(), O_RDONLY, 0);
    fstat(fd, &original_stat);
    close(fd);

    struct stat target_stat;
    fd = open(target.c_str(), O_RDONLY, 0);
    fstat(fd, &target_stat);
    close(fd);

    auto diff =
        badiff::Diff::Make(original_stream, original_stat.st_size,
                           target_stream, target_stat.st_size, chunk_size);

    std::ofstream delta_stream(delta);
    delta_stream.write(diff->diff.get(), diff->len);

    printf("\n");

  } else if (command == "apply") {
    if (argc != 5) {
      help();
      return EXIT_FAILURE;
    }
    std::ifstream original(*++arg);
    std::ifstream delta(*++arg);
    std::ofstream target(*++arg);

    badiff::q::OpQueue op_queue;
    op_queue.Deserialize(delta);
    op_queue.Apply(original, target);

  } else {
    help();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
