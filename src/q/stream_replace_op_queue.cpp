#include <badiff/q/stream_replace_op_queue.hpp>

namespace badiff {
extern bool CONSOLE_OUTPUT;

namespace q {

StreamReplaceOpQueue::StreamReplaceOpQueue(std::istream &original,
                                           int original_len,
                                           std::istream &target, int target_len,
                                           int max_chunk_size)
    : original_(original), target_(target), original_len_(original_len),
      target_len_(target_len) {
  int chunks = std::max(1, std::max(original_len, target_len) / max_chunk_size);
  original_chunk_size_ = std::max(1, original_len / chunks);
  target_chunk_size_ = std::max(1, target_len / chunks);
  original_pos_ = 0;
  target_pos_ = 0;
}

StreamReplaceOpQueue::~StreamReplaceOpQueue() {}

bool StreamReplaceOpQueue::Pull() {
  bool prepared = false;

  if (original_pos_ < original_len_) {
    std::unique_ptr<char[]> value(new char[original_chunk_size_]);
    int n = 0;
    do {
      original_.seekg(original_pos_++);
      value[n++] = original_.get();
    } while (n < original_chunk_size_ && original_pos_ < original_len_);
    Prepare(Op(Op::DELETE, n, std::move(value)));
    prepared = true;
    if (CONSOLE_OUTPUT) {
      printf("-");
      fflush(stdout);
    }
  }
  if (target_pos_ < target_len_) {
    std::unique_ptr<char[]> value(new char[target_chunk_size_]);
    int n = 0;
    do {
      target_.seekg(target_pos_++);
      value[n++] = target_.get();
    } while (n < target_chunk_size_ && target_pos_ < target_len_);
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
