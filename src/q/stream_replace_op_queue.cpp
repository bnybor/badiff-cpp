#include <badiff/q/stream_replace_op_queue.hpp>

namespace badiff {
extern bool CONSOLE_OUTPUT;

namespace q {

StreamReplaceOpQueue::StreamReplaceOpQueue(std::istream &original,
                                           int original_len,
                                           std::istream &target, int target_len,
                                           int max_chunk_size)
    : original_(original), target_(target) {
  int chunks = 1 + std::max(original_len, target_len) / max_chunk_size;
  original_chunk_size_ = original_len / chunks;
  target_chunk_size_ = target_len / chunks;
}

StreamReplaceOpQueue::~StreamReplaceOpQueue() {}

bool StreamReplaceOpQueue::Pull() {
  std::unique_ptr<char[]> original_buf(new char[original_chunk_size_]);
  std::unique_ptr<char[]> target_buf(new char[target_chunk_size_]);

  bool prepared = false;

  int n;
  if (!original_.eof() &&
      (n = original_.readsome(original_buf.get(), original_chunk_size_)) > 0) {
    std::unique_ptr<char[]> value(new char[n]);
    std::copy(original_buf.get(), original_buf.get() + n, value.get());
    Prepare(Op(Op::DELETE, n, std::move(value)));
    prepared = true;
    if (CONSOLE_OUTPUT) {
      printf("-");
      fflush(stdout);
    }
  }
  if (!target_.eof() &&
      (n = target_.readsome(target_buf.get(), target_chunk_size_)) > 0) {
    std::unique_ptr<char[]> value(new char[n]);
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
