#ifndef BADIFF_BADIFF_HPP_
#define BADIFF_BADIFF_HPP_

#include <istream>
#include <memory>
#include <ostream>

namespace badiff {

/**
 * \brief A computed diff.
 */
struct Diff {
  /**
   * \brief Compute a diff using two arrays of bytes.
   */
  static std::unique_ptr<Diff> Make(const char *original, int original_len,
                                    const char *target, int target_len);

  /**
   * \brief Compute a diff using two streams and their lengths.
   *
   * Adding the lengths helps badiff split the inputs into chunks more
   * effectively.
   */
  static std::unique_ptr<Diff> Make(std::istream &original, int original_len,
                                    std::istream &target, int target_len);

  /**
   * \brief The length of the original.
   */
  int original_len_;

  /**
   * \brief The length of the target.
   */
  int target_len_;

  /**
   * \brief The length of the diff.
   */
  int diff_len_;

  /**
   * \brief The diff itself.
   */
  std::unique_ptr<char[]> diff_;

  /**
   * \brief Apply a diff.
   */
  void Apply(std::istream &original, std::ostream &target);
  /**
   * \brief Apply a diff.
   */
  void Apply(const char *original, char *target);
};

} // namespace badiff

#endif /* BADIFF_BADIFF_HPP_ */
