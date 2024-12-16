#include <badiff/q/chunking_op_queue.hpp>

#include <algorithm>

namespace badiff {
namespace q {

ChunkingOpQueue::ChunkingOpQueue(std::unique_ptr<OpQueue> source,
                                 int chunk_size)
    : FilterOpQueue(std::move(source)), chunk_size_(chunk_size) {}

ChunkingOpQueue::~ChunkingOpQueue() {}

bool ChunkingOpQueue::FlushInterleaved(unsigned int n) {
  while (n-- > 0 && (!deletes_.empty() || !inserts_.empty())) {
    if (!deletes_.empty()) {
      Prepare(std::move(deletes_.front()));
      deletes_.erase(deletes_.begin());
    }
    if (!inserts_.empty()) {
      Prepare(std::move(inserts_.front()));
      inserts_.erase(inserts_.begin());
    }
  }
  return Flush();
}

bool ChunkingOpQueue::Pull() {
  if (!Require(1))
    return FlushInterleaved(-1);

  auto op(std::move(filtering_.front()));
  filtering_.erase(filtering_.begin());

  if (op.GetType() == Op::NEXT) {
    FlushInterleaved(-1);
    Prepare(std::move(op));
    return true;
  }

  for (int i = 0; i < op.GetLength(); i += chunk_size_) {
    int extent = std::min(op.GetLength() - i, chunk_size_);
    std::unique_ptr<char[]> value(new char[extent]);
    std::copy(op.GetValue().get() + i, op.GetValue().get() + i + extent,
              value.get());
    Op chunk(op.GetType(), extent, std::move(value));
    if (chunk.GetType() == Op::DELETE) {
      deletes_.push_back(std::move(chunk));
    } else {
      inserts_.push_back(std::move(chunk));
    }
  }

  return FlushInterleaved(1);
}

} // namespace q
} // namespace badiff
