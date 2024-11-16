#ifndef BADIFF_Q_OP_QUEUE_HPP_
#define BADIFF_Q_OP_QUEUE_HPP_

#include <vector>
#include <istream>
#include <ostream>

#include <badiff/diff.hpp>
#include <badiff/op.hpp>

namespace badiff {
namespace q {

class OpQueue {
protected:
	Op iter_next_;
	std::vector<Op> prepared_;
public:

	virtual Op poll();
	virtual bool Offer(Op);
	virtual void Drain();
	virtual std::vector<Op> & DrainTo(std::vector<Op>&);
	virtual OpQueue& DrainTo(OpQueue&);
	virtual Diff& DrainTo(Diff&);

protected:
	virtual bool Pull();
	virtual bool Prepare(const Op& e);
public:
	virtual void Apply(std::istream&, std::ostream&);
	virtual bool HasNext();
	virtual Op Next();
};

}
}



#endif /* BADIFF_Q_OP_QUEUE_HPP_ */
