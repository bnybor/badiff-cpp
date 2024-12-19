#include <badiff/q/graph_op_queue.hpp>

#include <iostream>

namespace badiff {
namespace q {

GraphOpQueue::GraphOpQueue(std::unique_ptr<OpQueue> source,
                           std::unique_ptr<alg::Graph> graph)
    : FilterOpQueue(std::move(source)), graph_(std::move(graph)) {}

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

  if (op0.GetType() == Op::DELETE && op1.GetType() == Op::INSERT) {
    delete_op = std::move(op0);
    insert_op = std::move(op1);
  } else if (op1.GetType() == Op::DELETE && op0.GetType() == Op::INSERT) {
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
