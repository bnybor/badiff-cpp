#include <badiff/q/coalescing_op_queue.hpp>

#include <iostream>

namespace badiff {
namespace q {

CoalescingOpQueue::CoalescingOpQueue(std::unique_ptr<OpQueue> source)
    : FilterOpQueue(std::move(source)) {}

std::unique_ptr<Op> CoalescingOpQueue::Pop() {
  if (filtering_.size() < 2)
    Pull();
  return FilterOpQueue::Pop();
}

bool CoalescingOpQueue::Pull() {
  if (!Require(2))
    return Flush(1);

  Op &op0 = filtering_[0];
  Op &op1 = filtering_[1];

  if (op0.GetType() != op1.GetType())
    return Flush(1);

  op0.MutableLength() += op1.GetLength();
  if (op0.GetValue() && op1.GetValue()) {
    auto &v0 = op0.GetValue();
    auto &l0 = op0.GetLength();
    auto &v1 = op1.GetValue();
    auto &l1 = op1.GetLength();
    std::unique_ptr<char[]> value(new char[l0 + l1]);
    std::copy(v0.get(), v0.get() + l0, value.get());
    std::copy(v1.get(), v1.get() + l1, value.get() + l0);
    op0.MutableValue() = std::move(value);
    op0.MutableLength() += op1.GetLength();
  } else {
    op0.MutableValue() = nullptr;
  }

  filtering_.erase(filtering_.begin() + 1);
  return true;
}

} // namespace q
} // namespace badiff
