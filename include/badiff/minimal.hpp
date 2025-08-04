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

#ifndef BADIFF_MINIMAL_HPP_
#define BADIFF_MINIMAL_HPP_

namespace badiff {

/**
 * \brief Compute a diff from original to target, and store in dst.
 *
 * \return The size of the diff, or -1 if the buffer was too small.
 */
int diff(char *dst, int dst_len, const char *original, int original_len,
         const char *target, int target_len);

/**
 * \brief Compute the target from original and diff, and store in dst.
 *
 * \return The size of the target, or -1 if the buffer was too small.
 */
int patch(char *dst, int dst_len, const char *original, int original_len,
          const char *diff, int diff_len);

} // namespace badiff

#endif