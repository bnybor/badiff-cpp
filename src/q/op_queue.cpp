#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

bool OpQueue::IsEmpty() {
	if (queue_.empty()) Pull();
	return queue_.empty();
}

Op OpQueue::PopFront() {
	if (queue_.empty()) Pull();
	if (queue_.empty()) return Op::STOP;
	Op op = std::move(queue_.front());
	queue_.erase(queue_.begin());
	return op;
}

void OpQueue::PushBack(Op op) {
	queue_.push_back(std::move(op));
}

void OpQueue::PushBack(OpQueue& other) {
	for (Op op = other.PopFront(); op.GetType() != Op::STOP; op = other.PopFront()) {
		PushBack(std::move(op));
	}
}

void OpQueue::Pull() {}

}
}
