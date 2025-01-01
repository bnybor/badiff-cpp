#include <badiff/q/replace_op_queue.hpp>

namespace badiff {
namespace q {

ReplaceOpQueue::ReplaceOpQueue(const char *original, int original_size,
                               const char *target, int target_size,
                               int max_chunk_size)
    : original_(original), target_(target), original_pos_(0),
      original_len_(original_size), target_pos_(0), target_len_(target_size) {
  int chunks =
      std::max(1, std::max(original_size, target_size) / max_chunk_size);
  original_chunk_len_ = std::max(1, original_size / chunks);
  target_chunk_len_ = std::max(1, target_size / chunks);
}

ReplaceOpQueue::~ReplaceOpQueue() {}

bool ReplaceOpQueue::Pull() {
  bool prepared = false;
  if (original_pos_ < original_len_) {
    int size = std::min(original_chunk_len_, original_len_ - original_pos_);
    std::unique_ptr<char[]> value(new char[size]);
    std::copy(original_ + original_pos_, original_ + original_pos_ + size,
              value.get());
    Prepare(Op(Op::DELETE, size, std::move(value)));
    original_pos_ += size;
    prepared = true;
  }
  if (target_pos_ < target_len_) {
    int size = std::min(target_chunk_len_, target_len_ - target_pos_);
    std::unique_ptr<char[]> value(new char[size]);
    std::copy(target_ + target_pos_, target_ + target_pos_ + size, value.get());
    Prepare(Op(Op::INSERT, size, std::move(value)));
    target_pos_ += size;
    prepared = true;
  }
  return prepared;
}

} // namespace q
} // namespace badiff
