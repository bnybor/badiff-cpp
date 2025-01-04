#ifndef BADIFF_BADIFF_HPP_
#define BADIFF_BADIFF_HPP_

#include <istream>
#include <memory>
#include <ostream>

namespace badiff {

/**
 * \brief A computed delta.
 */
struct Delta {
  /**
   * \brief Compute a delta using two arrays of bytes.
   */
  static std::unique_ptr<Delta> Make(const char *original, int original_len,
                                     const char *target, int target_len);

  /**
   * \brief Compute a delta using two streams and their lengths.
   *
   * Adding the lengths helps badiff split the inputs into chunks more
   * effectively.
   */
  static std::unique_ptr<Delta> Make(std::istream &original, int original_len,
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
   * \brief Serialize the delta to a stream, with versioning.
   */
  void Serialize(std::ostream &out) const;

  /**
   * \brief Deserialize the delta from a stream, with versioning.
   *
   * Returns false on failure.
   */
  bool Deserialize(std::istream &in);
};

} // namespace badiff

#endif /* BADIFF_BADIFF_HPP_ */
