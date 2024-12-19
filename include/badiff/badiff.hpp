#ifndef BADIFF_BADIFF_HPP_
#define BADIFF_BADIFF_HPP_

#include <istream>
#include <memory>
#include <ostream>

namespace badiff {

extern bool CONSOLE_OUTPUT;

struct Diff {
  static std::unique_ptr<Diff> Make(const char *original, int original_size,
                                    const char *target, int target_size,
                                    int chunk_size);
  static std::unique_ptr<Diff> Make(std::istream &original, int original_len,
                                    std::istream &target, int target_len,
                                    int chunk_size);

  static constexpr int VERY_FAST_CHUNK = 512;
  static constexpr int FAST_CHUNK = 1024;
  static constexpr int NORMAL_CHUNK = 2048;
  static constexpr int EFFICIENT_CHUNK = 4096;
  static constexpr int VERY_EFFICIENT_CHUNK = 8192;

  int len;
  std::unique_ptr<char[]> diff;

  void Apply(std::istream &original, std::ostream &target);
};

} // namespace badiff

#endif /* BADIFF_BADIFF_HPP_ */
