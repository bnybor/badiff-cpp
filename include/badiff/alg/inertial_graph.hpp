#ifndef BADIFF_ALG_EDIT_GRAPH_HPP_
#define BADIFF_ALG_EDIT_GRAPH_HPP_

#include <vector>

#include <badiff/op.hpp>
#include <badiff/alg/graph.hpp>

namespace badiff {
namespace alg {

class InertialGraph: public Graph {
	std::vector<Op> op_queue_;

public:
	// @formatter:off
	static constexpr std::size_t TRANSITION_COSTS[4][4] = //
			{ //
			{ 0, 2, 3, 2, }, // From STOP
			{ 2, 0, 3, 2, }, // From DELETE
			{ 2, 2, 1, 2, }, // From INSERT
			{ 2, 2, 3, 0, } // From NEXT
			}; // To S D I N
				// @formatter:on

	virtual ~InertialGraph() = default;

	void Compute(const Byte *original, std::size_t original_length,
			const Byte *target, std::size_t target_length) override;

	virtual std::unique_ptr<q::OpQueue> MakeOpQueue() const override;

};

}
}

#endif /* BADIFF_ALG_EDIT_GRAPH_HPP_ */
