#include <badiff/q/rstream_replace_op_queue.hpp>

namespace badiff {
extern bool CONSOLE_OUTPUT;

namespace q {

RStreamReplaceOpQueue::RStreamReplaceOpQueue(std::istream &original,
                                             int original_len,
                                             std::istream &target,
                                             int target_len, int max_chunk_size)
    : original_(original), target_(target) {
  int chunks = 1 + std::max(original_len, target_len) / max_chunk_size;
  original_chunk_size_ = original_len / chunks;
  target_chunk_size_ = target_len / chunks;
  original_pos_ = original_len;
  target_pos_ = target_len;
}

RStreamReplaceOpQueue::~RStreamReplaceOpQueue() {}

bool RStreamReplaceOpQueue::Pull() {
  std::unique_ptr<char[]> original_buf(new char[original_chunk_size_]);
  std::unique_ptr<char[]> target_buf(new char[target_chunk_size_]);

  bool prepared = false;

  int n;
  if (!original_pos_) {
    std::unique_ptr<char[]> value(new char[original_chunk_size_]);
    n = 0;
    do {
      original_.seekg(--original_pos_);
      value[n++] = original_.get();
    } while (n < original_chunk_size_ && original_pos_);
    std::copy(original_buf.get(), original_buf.get() + n, value.get());
    Prepare(Op(Op::DELETE, n, std::move(value)));
    prepared = true;
    if (CONSOLE_OUTPUT) {
      printf("-");
      fflush(stdout);
    }
  }
  if (target_pos_) {
    std::unique_ptr<char[]> value(new char[n]);
    n = 0;
    do {
      target_.seekg(--target_pos_);
      value[n++] = target_.get();
    } while (n < original_chunk_size_ && original_pos_);
    std::copy(target_buf.get(), target_buf.get() + n, value.get());
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
