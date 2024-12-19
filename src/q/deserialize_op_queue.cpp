#include <badiff/q/deserialize_op_queue.hpp>

namespace badiff {
namespace q {

DeserializeOpQueue::DeserializeOpQueue(std::istream &delta)
    : delta_(delta), has_next_(true) {}

bool DeserializeOpQueue::Pull() {
  if (!has_next_)
    return false;
  Op op;
  op.Deserialize(delta_);
  if (op.GetType() == Op::STOP) {
    has_next_ = false;
    return false;
  }
  Prepare(std::move(op));
  return true;
}

} // namespace q
} // namespace badiff
