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

#include <badiff/q/graph_op_queue.hpp>

#include <iostream>

namespace badiff {
namespace q {

GraphOpQueue::GraphOpQueue(std::unique_ptr<OpQueue> source,
                           std::unique_ptr<alg::Graph> graph, Pairing pairing)
    : FilterOpQueue(std::move(source)), graph_(std::move(graph)),
      pairing_(pairing) {}

GraphOpQueue::~GraphOpQueue() {}

bool GraphOpQueue::Pull() {
  if (!Require(2))
    return Flush(1);

  Op &op0 = filtering_[0];
  Op &op1 = filtering_[1];

  if (!op0.GetValue() || !op1.GetValue())
    return Flush(1);

  Op delete_op;
  Op insert_op;

  if (op0.GetType() == Op::DELETE && op1.GetType() == Op::INSERT &&
      pairing_ != INSERT_THEN_DELETE) {
    delete_op = std::move(op0);
    insert_op = std::move(op1);
  } else if (op1.GetType() == Op::DELETE && op0.GetType() == Op::INSERT &&
             pairing_ != DELETE_THEN_INSERT) {
    delete_op = std::move(op1);
    insert_op = std::move(op0);
  } else
    return Flush(1);

  filtering_.erase(filtering_.begin());
  filtering_.erase(filtering_.begin());

  graph_->Compute(delete_op.GetValue().get(), delete_op.GetLength(),
                  insert_op.GetValue().get(), insert_op.GetLength());

  auto diff = graph_->MakeOpQueue();

  Op op;
  while (!diff->IsEmpty()) {
    op = *diff->Pop();
    if (diff->IsEmpty() && op.GetType() != Op::NEXT) {
      filtering_.insert(filtering_.begin(), std::move(op));
      break;
    }
    Prepare(std::move(op));
  }

  if (filtering_[0].GetType() == Op::NEXT) {
    Flush(1);
  }

  return true;
}

} // namespace q
} // namespace badiff
