#include <badiff/q/chunking_op_queue.hpp>

#include <algorithm>

namespace badiff {
namespace q {

ChunkingOpQueue::ChunkingOpQueue(std::unique_ptr<OpQueue> source,
                                 int chunk_size)
    : FilterOpQueue(std::move(source)), chunk_size_(chunk_size) {}

ChunkingOpQueue::~ChunkingOpQueue() {}

bool ChunkingOpQueue::Pull() {
  if (!Require(1))
    return false;

  auto op(std::move(filtering_.front()));
  filtering_.erase(filtering_.begin());

  if (op.GetLength() <= chunk_size_) {
    Prepare(std::move(op));
    return true;
  }

  for (int i = 0; i < op.GetLength(); i += chunk_size_) {
    int extent = std::min(op.GetLength() - i, chunk_size_);
    std::unique_ptr<char[]> value(new char[extent]);
    std::copy(op.GetValue().get() + i, op.GetValue().get() + i + extent,
              value.get());
    Prepare(Op(op.GetType(), extent, std::move(value)));
  }

  return true;
}

} // namespace q
} // namespace badiff
