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

#include <badiff/q/rewinding_op_queue.hpp>

namespace badiff {
namespace q {

RewindingOpQueue::RewindingOpQueue(std::unique_ptr<OpQueue> op_queue)
    : FilterOpQueue(std::move(op_queue)) {}

bool RewindingOpQueue::Pull() {
  while (Require(1)) {
    Op op = std::move(filtering_[0]);
    filtering_.erase(filtering_.begin());

    if (op.GetType() == Op::NEXT) {
      FlushHistory();
      Prepare(std::move(op));
      return true;
    }

    for (auto op2 = history_.begin(); op2 != history_.end(); ++op2) {
      if (op.GetType() == op2->GetType() ||
          op.GetLength() != op2->GetLength() ||
          !std::equal(op.GetValue().get(), op.GetValue().get() + op.GetLength(),
                      op2->GetValue().get()))
        continue;
      for (auto dump = history_.begin(); dump < op2; ++dump) {
        Prepare(std::move(*dump));
      }
      history_.erase(history_.begin(), op2 + 1);
      Prepare(Op(Op::NEXT, op.GetLength()));
      return true;
    }
    history_.push_back(std::move(op));
  }
  return FlushHistory();
}

bool RewindingOpQueue::FlushHistory() {
  bool flushed = false;
  for (auto &op : history_) {
    flushed = true;
    Prepare(std::move(op));
  }
  history_.clear();
  return flushed;
}

} // namespace q
} // namespace badiff
