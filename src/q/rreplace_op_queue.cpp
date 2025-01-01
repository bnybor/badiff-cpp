#include <badiff/q/rreplace_op_queue.hpp>

namespace badiff {
extern bool CONSOLE_OUTPUT;

namespace q {

RReplaceOpQueue::RReplaceOpQueue(const char *original, int original_len,
                                 const char *target, int target_len,
                                 int max_chunk_len)
    : original_(original), target_(target), original_len_(original_len),
      target_len_(target_len) {
  int chunks = std::max(1, std::max(original_len, target_len) / max_chunk_len);
  original_chunk_len_ = std::max(1, original_len / chunks);
  target_chunk_len_ = std::max(1, target_len / chunks);
  original_pos_ = original_len;
  target_pos_ = target_len;
}

RReplaceOpQueue::~RReplaceOpQueue() {}

bool RReplaceOpQueue::Pull() {
  bool prepared = false;

  if (original_pos_) {
    std::unique_ptr<char[]> value(new char[original_chunk_len_]);
    int n = 0;
    do {
      value[n++] = original_[--original_pos_];
    } while (n < original_chunk_len_ && original_pos_);
    Prepare(Op(Op::DELETE, n, std::move(value)));
    prepared = true;
    if (CONSOLE_OUTPUT) {
      printf("-");
      fflush(stdout);
    }
  }
  if (target_pos_) {
    std::unique_ptr<char[]> value(new char[target_chunk_len_]);
    int n = 0;
    do {
      value[n++] = target_[--target_pos_];
    } while (n < target_chunk_len_ && target_pos_);
    Prepare(Op(Op::INSERT, n, std::move(value)));
    prepared = true;
    if (CONSOLE_OUTPUT) {
      printf("+");
      fflush(stdout);
    }
  }

  return prepared;
}

} // namespace q
} // namespace badiff
