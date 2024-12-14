#ifndef BADIFF_BADIFF_HPP_
#define BADIFF_BADIFF_HPP_

#include <memory>

namespace badiff {

struct Diff {
  static std::unique_ptr<Diff> MakeDiff(const char *original, int original_size,
                                        const char *target, int target_size);

  // The length of the serialized OpQueue
  int len;
  // The serialized OpQueue
  std::unique_ptr<char[]> op_queue;
};

} // namespace badiff

#endif /* BADIFF_BADIFF_HPP_ */
