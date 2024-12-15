#include <badiff/q/stream_replace_op_queue.hpp>

namespace badiff {
namespace q {

StreamReplaceOpQueue::StreamReplaceOpQueue(std::istream &original,
                                           std::istream &target, int chunk_size)
    : original_(original), target_(target), chunk_size_(chunk_size) {}

StreamReplaceOpQueue::~StreamReplaceOpQueue() {}

bool StreamReplaceOpQueue::Pull() {
  char buf[chunk_size_];

  bool prepared = false;

  int n;
  if (!original_.eof() && (n = original_.readsome(buf, chunk_size_)) > 0) {
    std::unique_ptr<char[]> value(new char[n]);
    std::copy(buf, buf + n, value.get());
    Prepare(Op(Op::DELETE, n, std::move(value)));
    prepared = true;
    printf("-");
    fflush(stdout);
  }
  if (!target_.eof() && (n = target_.readsome(buf, chunk_size_)) > 0) {
    std::unique_ptr<char[]> value(new char[n]);
    std::copy(buf, buf + n, value.get());
    Prepare(Op(Op::INSERT, n, std::move(value)));
    prepared = true;
    printf("+");
    fflush(stdout);
  }

  return prepared;
}

} // namespace q
} // namespace badiff
