#include <badiff/q/coalescing_op_queue.hpp>

#include <iostream>

namespace badiff {
namespace q {

CoalescingOpQueue::CoalescingOpQueue(std::unique_ptr<OpQueue> source)
    : FilterOpQueue(std::move(source)) {}

bool CoalescingOpQueue::Pull() {
  while (Require(2)) {

    if (Require(3)) {
      // Might be able to reorder a triple that has only INSERT and DELETE for
      // coalescing.
      Op &op0 = filtering_[0];
      Op &op1 = filtering_[1];
      Op &op2 = filtering_[2];
      bool reorder = true;
      if (op0.GetType() == Op::NEXT || op1.GetType() == Op::NEXT ||
          op2.GetType() == Op::NEXT)
        reorder = false;
      if (op0.GetType() == op1.GetType())
        reorder = false;
      if (reorder) {
        if (op0.GetType() == op2.GetType()) {
          Op tmp = std::move(op1);
          op1 = std::move(op2);
          op2 = std::move(tmp);
        } else if (op1.GetType() == op2.GetType()) {
          Op tmp = std::move(op0);
          op0 = std::move(op1);
          op1 = std::move(op2);
          op2 = std::move(tmp);
        }
      }
    }

    Op &op0 = filtering_[0];
    Op &op1 = filtering_[1];

    if (op0.GetType() != op1.GetType()) {
      if (op0.GetType() == Op::INSERT && op1.GetType() == Op::DELETE) {
        Op tmp = std::move(op0);
        op0 = std::move(op1);
        op1 = std::move(tmp);
      }
      return Flush(1);
    }

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
  }
  return Flush(1);
}

} // namespace q
} // namespace badiff
