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
      --argv;
    } else {
      --arg;
    }

    if (argc != 5) {
      help();
      return EXIT_FAILURE;
    }

    std::string original(*++arg);
    std::string target(*++arg);
    std::string delta(*++arg);

    auto diff = badiff::Delta::Make(original, target);

    if (delta == "-") {
      diff->Serialize(std::cout);
    } else {
      diff->Serialize(delta);
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

  } else {
    help();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
