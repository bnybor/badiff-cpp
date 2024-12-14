#include <badiff/q/minimize_op_queue.hpp>

#include <algorithm>

namespace badiff {
namespace q {

MinimizeOpQueue::MinimizeOpQueue(std::unique_ptr<OpQueue> source)
    : FilterOpQueue(std::move(source)) {}

MinimizeOpQueue::~MinimizeOpQueue() {}

bool MinimizeOpQueue::Pull() {
  if (!Require(1))
    return false;

  auto op(std::move(filtering_.front()));
  filtering_.erase(filtering_.begin());

  if (op.GetType() != Op::INSERT) {
    op.MutableValue() = nullptr;
  }

  Prepare(std::move(op));

  return true;
}

} // namespace q
} // namespace badiff
