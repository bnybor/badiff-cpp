#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

OpQueue::OpQueue(std::vector<Op> queue) :
		queue_(std::move(queue)) {
}

bool OpQueue::IsEmpty() {
	if (queue_.empty())
		Pull();
	return queue_.empty();
}

Op OpQueue::PopFront() {
	if (queue_.empty())
		Pull();
	if (queue_.empty())
		return Op::STOP;
	Op op = std::move(queue_.front());
	queue_.erase(queue_.begin());
	return op;
}

void OpQueue::PushBack(Op op) {
	queue_.push_back(std::move(op));
}

void OpQueue::PushBack(OpQueue &other) {
	for (Op op = other.PopFront(); op.GetType() != Op::STOP; op =
			other.PopFront()) {
		PushBack(std::move(op));
	}
}

void OpQueue::Pull() {
}

std::string OpQueue::SummarizeConsuming(OpQueue& op_queue) {
	std::ostringstream ss;
	while (!op_queue.IsEmpty()) {
		Op op = op_queue.PopFront();
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

}
}
