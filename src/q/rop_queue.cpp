#include <badiff/q/rop_queue.hpp>

#include <algorithm>

namespace badiff {
namespace q {

ROpQueue::ROpQueue(std::unique_ptr<OpQueue> source)
    : FilterOpQueue(std::move(source)) {}

ROpQueue::~ROpQueue() {}

bool ROpQueue::Pull() {
  if (!Require(1))
    return false;

  auto op(std::move(filtering_.front()));
  filtering_.erase(filtering_.begin());

  if (op.GetValue()) {
    std::unique_ptr<char[]> reverse(new char[op.GetLength()]);
    for (int i = 0; i < op.GetLength(); i++) {
      reverse[op.GetLength() - 1 - i] = op.GetValue()[i];
    }
    op.MutableValue() = std::move(reverse);
  }

  Prepare(std::move(op));

  return true;
}

} // namespace q
} // namespace badiff
