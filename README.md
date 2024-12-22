# badiff-cpp

C++ port of the [badiff](https://github.com/org-badiff/badiff) binary diffing
tool.

# Quick Start

**Usage**

```
$ ./build/badiff
badiff diff [-v] <original> <target> <delta>
Create a delta from original to target.
      -v                  Verbose output
      <original>          Original file
      <target>            Target file
      <delta>             Delta file

badiff apply <original> <delta> <target>
Apply a delta file from original to target.
      <original>          Original file
      <target>            Target file
      <delta>             Delta file
```

**Compile the code**

```
mkdir build
cd build
cmake ..
make
```

**Install**

```
sudo make install
```

**Debug builds**

```
make clean
make
make test
```

**Output files:**

-   `build/badiff` Badiff as a runnable tool.
-   `build/libbadiff.so` Badiff as a shared library.
-   `build/test/badiff_test` Unit tests.

**Example:**

This example:
1.  Computes a delta from `README.md` to `LICENSE.txt`.
2.  Recomputes `LICENSE.txt` from `README.md` and the delta.
3.  Compares the two `LICENSE.txt` files, finding no differences.
4.  Shows the sizes of the files involved.

```
$ ./build/badiff diff README.md LICENSE.txt /tmp/readme_to_license.delta
$ ./build/badiff apply README.md /tmp/readme_to_license.delta /tmp/LICENSE2.txt
$ diff LICENSE.txt /tmp/LICENSE2.txt
$ ls -l README.md LICENSE.txt /tmp/readme_to_license.delta /tmp/LICENSE2.txt
-rw-r--r-- 1 robyn robyn 1270 Dec 19 09:55 /tmp/LICENSE2.txt
-rw-r--r-- 1 robyn robyn   16 Dec 19 09:54 /tmp/readme_to_license.delta
-rw-r--r-- 1 robyn robyn 1270 Dec 19 09:27 LICENSE.txt
-rw-r--r-- 1 robyn robyn 1723 Dec 19 09:46 README.md
```

The diff is 16 bytes.

# License

BSD-style two-clause license.

See [LICENSE.txt](LICENSE.txt)

Copyright 2024 Robyn Kirkman

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

