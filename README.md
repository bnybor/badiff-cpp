# badiff-cpp

C++ port of the [badiff](https://github.com/org-badiff/badiff) binary diffing
tool.

# Quick Start

**Usage**

```
badiff: Binary diffing and patching tool tool.

badiff diff [-v] <original> <target> <delta>
Create a delta from original to target.
      -v                  Verbose output
      <original>          Original file
      <target>            Target file
      <delta>             Delta file, '-' for stdout

badiff patch <original> <target> <delta>
Apply a delta file from original to target.
      <original>          Original file
      <target>            Target file
      <delta>             Delta file, '-' for stdin
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

```
$ ./build/badiff diff README.md LICENSE.txt /tmp/readme_to_license.delta
$ ./build/badiff apply README.md /tmp/readme_to_license.delta /tmp/LICENSE2.txt
```

The diff is 16 bytes.

# License

MIT license.

See [LICENSE.txt](LICENSE.txt)

Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.