/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <badiff/q/coalescing_op_queue.hpp>

#include <iostream>

namespace badiff {
namespace q {

CoalescingOpQueue::CoalescingOpQueue(std::unique_ptr<OpQueue> source)
    : FilterOpQueue(std::move(source)) {}

CoalescingOpQueue::~CoalescingOpQueue() {}

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
      if (op0.GetType() == Op::DELETE && op1.GetType() == Op::INSERT &&
          op0.GetLength() == op1.GetLength() && op0.GetValue() &&
          op1.GetValue() &&
          std::equal(op0.GetValue().get(),
                     op0.GetValue().get() + op0.GetLength(),
                     op1.GetValue().get())) {
        // Replace identical DELETE,INSERT with NEXT
        Prepare(Op(Op::NEXT, op0.GetLength()));
        filtering_.erase(filtering_.begin(), filtering_.begin() + 2);
        return true;
      }
    }
    return Flush(1);
  }
  return Flush(1);
}

} // namespace q
} // namespace badiff
