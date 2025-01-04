#include <fstream>
#include <iostream>
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
  printf("badiff: Binary diffing and patching tool tool.\n");
  printf("\n");
  printf("badiff diff [-v] <original> <target> <delta>\n"
         "Create a delta from original to target.\n"
         "      -v                  Verbose output\n"
         "      <original>          Original file\n"
         "      <target>            Target file\n"
         "      <delta>             Delta file, '-' for stdout\n"
         "\n"
         "badiff patch <original> <target> <delta>\n"
         "Apply a delta file from original to target.\n"
         "      <original>          Original file\n"
         "      <target>            Target file\n"
         "      <delta>             Delta file, '-' for stdin\n");
}

int main(int argc, const char **argv) {
  using badiff::Delta;

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

    auto diff = badiff::Delta::Make(original_mmap, original_stat.st_size,
                                    target_mmap, target_stat.st_size);

    if (delta == "-") {
      diff->Serialize(std::cout);
    } else {
      std::ofstream delta_stream(delta);
      diff->Serialize(delta_stream);
    }

    if (badiff::CONSOLE_OUTPUT)
      printf("\n");

  } else if (command == "patch") {
    if (argc != 5) {
      help();
      return EXIT_FAILURE;
    }

    std::string original(*++arg);
    std::string target(*++arg);
    std::string delta(*++arg);

    std::unique_ptr<badiff::Delta> diff(new badiff::Delta);

    std::ifstream original_stream(original);
    if (delta == "-") {
      if (!diff->Deserialize(std::cin)) {
        fprintf(stderr, "Bad diff.\n");
        return EXIT_FAILURE;
      }
    } else {
      std::ifstream delta_stream(delta);
      if (!diff->Deserialize(delta_stream)) {
        fprintf(stderr, "Bad diff.\n");
        return EXIT_FAILURE;
      }
    }

    std::ofstream target_stream(target);
    diff->Apply(original_stream, target_stream);

  } else {
    help();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
