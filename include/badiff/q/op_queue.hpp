#ifndef BADIFF_Q_OP_QUEUE_HPP_
#define BADIFF_Q_OP_QUEUE_HPP_

#include <istream>
#include <ostream>
#include <memory>
#include <vector>

namespace badiff {
namespace q {
class OpQueue;
}
}

#include <badiff/op.hpp>

namespace badiff {
namespace q {

class OpQueue {
protected:
	std::vector<Op> queue_;

public:
	virtual ~OpQueue() = default;

	OpQueue() = default;
	OpQueue(const OpQueue&) = delete;
	OpQueue(OpQueue&&) = default;
	OpQueue& operator=(const OpQueue&) = delete;
	OpQueue& operator=(OpQueue&&) = default;

	OpQueue(std::vector<Op>);

	virtual bool IsEmpty();
	virtual Op PopFront();
	virtual void PushBack(Op);
	virtual void PushBack(OpQueue&);

protected:
	virtual void Pull();
};

}
}

#endif /* BADIFF_Q_OP_QUEUE_HPP_ */
