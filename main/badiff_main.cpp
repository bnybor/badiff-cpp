/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <badiff/badiff.hpp>

static int help(int status) {
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
  return status;
}

int main(int argc, const char **argv) {
  using badiff::Delta;

  mallopt(M_MMAP_THRESHOLD, 128 * 1024 * 1024);

  struct {
    bool verbose_ = false;
    bool positional_only_ = false;
  } flags;
  std::vector<std::string> positional_args;

  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if (flags.positional_only_)
      positional_args.push_back(arg);
    else if (arg == "-v" || arg == "--verbose")
      flags.verbose_ = true;
    else if (arg == "--")
      flags.positional_only_ = true;
    else
      positional_args.push_back(arg);
  }

  std::function<void(int original_pos, int target_pos)> *reporter = nullptr;

  std::function<void(int original_pos, int target_pos)> verbose_reporter(
      [](int original_pos, int target_pos) {
        printf("%i %i\n", original_pos, target_pos);
      });

  if (flags.verbose_) {
    reporter = &verbose_reporter;
  }

  if (positional_args.empty()) {
    return help(EXIT_FAILURE);
  }

  std::string command = positional_args[0];
  if (command == "help") {
    if (positional_args.size() != 1)
      return help(EXIT_FAILURE);
    return help(EXIT_SUCCESS);
  } else if (command == "diff") {
    if (positional_args.size() != 4)
      return help(EXIT_FAILURE);

    std::string original = positional_args[1];
    std::string target = positional_args[2];
    std::string delta = positional_args[3];

    auto diff = badiff::Delta::Make(original, target, reporter);

    if (delta == "-") {
      diff->Serialize(std::cout);
    } else {
      diff->Serialize(delta);
    }

    if (flags.verbose_)
      printf("Done.\n");

    return EXIT_SUCCESS;
  } else if (command == "patch" && positional_args.size() == 4) {
    if (positional_args.size() != 4)
      return help(EXIT_FAILURE);

    std::string original = positional_args[1];
    std::string target = positional_args[2];
    std::string delta = positional_args[3];

    std::unique_ptr<badiff::Delta> diff(new badiff::Delta);

    if (delta == "-") {
      if (!diff->Deserialize(std::cin)) {
        fprintf(stderr, "Bad diff.\n");
        return EXIT_FAILURE;
      }
    } else {
      if (!diff->Deserialize(delta)) {
        fprintf(stderr, "Bad diff.\n");
        return EXIT_FAILURE;
      }
    }

    diff->Apply(original, target);

    return EXIT_SUCCESS;
  } else {
    return help(EXIT_FAILURE);
  }
}
