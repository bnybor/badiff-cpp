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

#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

OpQueue::~OpQueue() {}

std::unique_ptr<Op> OpQueue::Pop() {
  if (prepared_.empty())
    Pull();
  if (prepared_.empty())
    return nullptr;
  auto ret = std::move(prepared_.front());
  prepared_.erase(prepared_.begin());
  return std::unique_ptr<Op>(new Op(ret));
}

bool OpQueue::IsEmpty() {
  if (prepared_.empty())
    Pull();
  return prepared_.empty();
}

void OpQueue::Push(Op op) { prepared_.push_back(std::move(op)); }

bool OpQueue::Pull() { return false; }

void OpQueue::Prepare(Op op) { prepared_.push_back(std::move(op)); }

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

void OpQueue::Serialize(std::ostream &out) {
  while (!IsEmpty()) {
    Op op = *Pop();
    op.Serialize(out);
  }
  Op(Op::STOP).Serialize(out);
}
void OpQueue::Deserialize(std::istream &in) {
  while (true) {
    Op op;
    op.Deserialize(in);
    if (op.GetType() == Op::STOP)
      break;
    Push(op);
  }
}

void OpQueue::Apply(std::istream &original, std::ostream &target) {
  while (!IsEmpty()) {
    Op op = *Pop();
    if (op.GetType() == Op::STOP)
      break;
    op.Apply(original, target);
  }
}

} // namespace q
} // namespace badiff
