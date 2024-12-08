#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

std::unique_ptr<Op> OpQueue::Pop() {
  if (prepared_.empty()) Pull();
  if (prepared_.empty()) return nullptr;
  auto ret = std::move(prepared_.front());
  prepared_.erase(prepared_.begin());
  return std::unique_ptr<Op>(new Op(ret));
}

bool OpQueue::IsEmpty() {
  if (prepared_.empty()) Pull();
  return prepared_.empty();
}

void OpQueue::Push(Op op) { prepared_.push_back(std::move(op)); }

bool OpQueue::Pull() { return false; }

void OpQueue::Prepare(Op op) { Push(std::move(op)); }

std::string OpQueue::SummarizeConsuming(OpQueue &op_queue) {
  std::ostringstream ss;
  while (!op_queue.IsEmpty()) {
    Op op = *op_queue.Pop();
    switch (op.GetType()) {
      case Op::STOP:
        ss << "!";
        break;
      case Op::DELETE:
        ss << "-" << op.GetLength();
        break;
      case Op::INSERT:
        ss << "+" << op.GetLength();
        break;
      case Op::NEXT:
        ss << ">" << op.GetLength();
        break;
    }
  }
  return ss.str();
}

}  // namespace q
}  // namespace badiff
