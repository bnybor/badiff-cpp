#ifndef BADIFF_ALG_GRAPH_HPP_
#define BADIFF_ALG_GRAPH_HPP_

#include <badiff/bytes.hpp>
#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace alg {

class Graph {
public:
	/**
	 * \brief Compute an OpQueue that transforms `original` into `target`.
	 */
	virtual void Compute(ByteArray original, ByteArray target) = 0;

	/**
	 * \brief Return a copy of the OpQueue computed by `Compute`.
	 */
	virtual std::unique_ptr<q::OpQueue> OpQueue() = 0;
};

}
}



#endif /* BADIFF_ALG_GRAPH_HPP_ */
