#include <badiff/q/compacting_op_queue.hpp>

#include <iostream>

namespace badiff {
namespace q {

CompactingOpQueue::CompactingOpQueue(std::unique_ptr<OpQueue> source)
    : FilterOpQueue(std::move(source)) {}

CompactingOpQueue::~CompactingOpQueue() {}

bool CompactingOpQueue::Pull() {
  while (Require(2)) {

    Op &op0 = filtering_[0];
    Op &op1 = filtering_[1];

    if (op0.GetType() != op1.GetType()) {
      return Flush(1);
    }

    if (op0.GetValue() && op1.GetValue()) {
      auto &v0 = op0.GetValue();
      auto &l0 = op0.GetLength();
      auto &v1 = op1.GetValue();
      auto &l1 = op1.GetLength();
      std::unique_ptr<char[]> value(new char[l0 + l1]);
      std::copy(v0.get(), v0.get() + l0, value.get());
      std::copy(v1.get(), v1.get() + l1, value.get() + l0);
      op0.MutableValue() = std::move(value);
    } else {
      op0.MutableValue() = nullptr;
    }
    op0.MutableLength() += op1.GetLength();

    filtering_.erase(filtering_.begin() + 1);
  }
  return Flush(1);
}

} // namespace q
} // namespace badiff
