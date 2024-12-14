#ifndef BADIFF_BADIFF_HPP_
#define BADIFF_BADIFF_HPP_

#include <memory>

namespace badiff {

struct Diff {
  int len;
  std::unique_ptr<char[]> diff;
};

std::unique_ptr<Diff> MakeDiff(const char *original, int original_size,
                               const char *target, int target_size);

} // namespace badiff

#endif /* BADIFF_BADIFF_HPP_ */
