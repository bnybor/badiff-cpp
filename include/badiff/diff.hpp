#ifndef BADIFF_DIFF_HPP_
#define BADIFF_DIFF_HPP_

namespace badiff {
class Diff;
}

#include <badiff/q/op_queue.hpp>

namespace badiff {
class Diff {
public:
	virtual ~Diff() = default;

	virtual void Store(q::OpQueue& out) = 0;

};
}



#endif /* BADIFF_DIFF_HPP_ */
