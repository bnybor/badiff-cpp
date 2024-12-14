#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>

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
    std::string original(argv[2]);
    std::string target(argv[3]);
    std::string diff(argv[4]);

    int original_fd = open(original.c_str(), O_RDONLY);
    struct stat original_stat;
    fstat(original_fd, &original_stat);
    const char *original_mmap = mmap(NULL, original_stat.st_size, PROT_READ,
                                     MAP_SHARED, original_fd, 0);

    int target_fd = open(target.c_str(), O_RDONLY);
    struct stat target_stat;
    fstat(target_fd, &target_stat);
    const char *target_mmap =
        mmap(NULL, target_stat.st_size, PROT_READ, MAP_SHARED, target_fd, 0);

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
