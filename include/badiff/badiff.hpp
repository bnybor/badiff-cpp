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

#ifndef BADIFF_BADIFF_HPP_
#define BADIFF_BADIFF_HPP_

#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

namespace badiff {

/**
 * \brief A computed delta.
 */
struct Delta {
  /**
   * \brief Compute a delta using two arrays of bytes.
   */
  static std::unique_ptr<Delta>
  Make(const char *original, int original_len, const char *target,
       int target_len,
       std::function<void(int original_pos, int target_pos, int original_len,
                          int target_len)> *reporter = nullptr);

  /**
   * \brief Compute a delta using two streams and their lengths.
   *
   * The streams must support istream::seekg.
   */
  static std::unique_ptr<Delta>
  Make(std::istream &original, int original_len, std::istream &target,
       int target_len,
       std::function<void(int original_pos, int target_pos, int original_len,
                          int target_len)> *reporter = nullptr);

  /**
   * \brief Memory-maps files and computes the diff.
   *
   * Computes the diff using Delta::Make(const char*, int, const char*, int).
   */
  static std::unique_ptr<Delta>
  Make(std::string original_file, std::string target_file,
       std::function<void(int original_pos, int target_pos, int original_len,
                          int target_len)> *reporter = nullptr);

  /**
   * \brief The length of the original.
   */
  int original_len_;

  /**
   * \brief The length of the target.
   */
  int target_len_;

  /**
   * \brief The length of the delta.
   */
  int delta_len_;

  /**
   * \brief The delta itself.
   */
  std::unique_ptr<char[]> delta_;

  /**
   * \brief Apply a delta.
   */
  void Apply(std::istream &original, std::ostream &target);
  /**
   * \brief Apply a delta.
   */
  void Apply(const char *original, char *target);

  /**
   * \brief Apply a delta to files.
   *
   * Opens files as std::ifstream and std::ofstream.
   */
  void Apply(std::string original_file, std::string target_file);

  /**
   * \brief Serialize the delta to a stream, with versioning.
   */
  void Serialize(std::ostream &out) const;

  /**
   * \brief Serialize a diff to a file.
   *
   * Opens the file as std::ofstream.
   */
  void Serialize(std::string delta_file) const;

  /**
   * \brief Deserialize the delta from a stream, with versioning.
   *
   * Returns false on failure.
   */
  bool Deserialize(std::istream &in);

  /**
   * \brief Deserializes a diff from a file.
   *
   * Opens the file as std::ifstream.
   */
  bool Deserialize(std::string delta_file);
};

} // namespace badiff

#endif /* BADIFF_BADIFF_HPP_ */
