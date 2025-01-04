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

namespace badiff {
extern bool CONSOLE_OUTPUT;
}

static void help() {
  printf("badiff diff [-v] <original> <target> <delta>\n"
         "Create a delta from original to target.\n"
         "      -v                  Verbose output\n"
         "      <original>          Original file\n"
         "      <target>            Target file\n"
         "      <delta>             Delta file\n"
         "\n"
         "badiff apply <original> <delta> <target>\n"
         "Apply a delta file from original to target.\n"
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

  mallopt(M_MMAP_THRESHOLD, 128 * 1024 * 1024);

  const char **arg = argv;
  std::string command(*++arg);

  if (command == "help") {
    help();
    return EXIT_SUCCESS;
  } else if (command == "diff") {
    if (argc < 5 && argc > 6) {
      help();
      return EXIT_FAILURE;
    }

    if (std::string(*++arg) == "-v") {
      badiff::CONSOLE_OUTPUT = true;
    } else {
      --arg;
    }

    std::string original(*++arg);
    std::string target(*++arg);
    std::string delta(*++arg);

    struct stat original_stat;
    int original_fd;
    original_fd = open(original.c_str(), O_RDONLY, 0);
    fstat(original_fd, &original_stat);

    struct stat target_stat;
    int target_fd;
    target_fd = open(target.c_str(), O_RDONLY, 0);
    fstat(target_fd, &target_stat);

    int original_size = original_stat.st_size;
    int target_size = target_stat.st_size;

    const char *original_mmap = (const char *)mmap(
        NULL, original_size, PROT_READ, MAP_PRIVATE, original_fd, 0);
    const char *target_mmap = (const char *)mmap(NULL, target_size, PROT_READ,
                                                 MAP_PRIVATE, target_fd, 0);

    auto diff = badiff::Diff::Make(original_mmap, original_stat.st_size,
                                   target_mmap, target_stat.st_size);

    std::ofstream delta_stream(delta);
    diff->Serialize(delta_stream);

    if (badiff::CONSOLE_OUTPUT)
      printf("\n");

  } else if (command == "apply") {
    if (argc != 5) {
      help();
      return EXIT_FAILURE;
    }
    std::ifstream original(*++arg);
    std::ifstream delta(*++arg);

    std::unique_ptr<badiff::Diff> diff(new badiff::Diff);
    if (!diff->Deserialize(delta)) {
      fprintf(stderr, "Bad diff.\n");
      return EXIT_FAILURE;
    }

    std::ofstream target(*++arg);
    diff->Apply(original, target);

  } else {
    help();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
