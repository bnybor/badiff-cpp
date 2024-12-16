#ifndef BADIFF_BADIFF_HPP_
#define BADIFF_BADIFF_HPP_

#include <istream>
#include <memory>
#include <ostream>

namespace badiff {

extern bool CONSOLE_OUTPUT;

struct Diff {
  static std::unique_ptr<Diff> Make(const char *original, int original_size,
                                    const char *target, int target_size);
  static std::unique_ptr<Diff> Make(std::istream &original, int original_len,
                                    std::istream &target, int target_len);

  int len;
  std::unique_ptr<char[]> diff;

  void Apply(std::istream &original, std::ostream &target);
};

} // namespace badiff

#endif /* BADIFF_BADIFF_HPP_ */
