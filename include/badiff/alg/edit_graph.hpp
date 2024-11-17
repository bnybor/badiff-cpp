#ifndef BADIFF_ALG_EDIT_GRAPH_HPP_
#define BADIFF_ALG_EDIT_GRAPH_HPP_

#include <vector>

#include <badiff/op.hpp>
#include <badiff/alg/graph.hpp>

namespace badiff {
namespace alg {

class EditGraph : public Graph {
	std::vector<Op> op_queue_;
public:
	virtual ~EditGraph() = default;

	void Compute(const Byte *original, std::size_t original_length,
			const Byte *target, std::size_t target_length) override;

	virtual std::unique_ptr<q::OpQueue> MakeOpQueue() const override;


};

}
}



#endif /* BADIFF_ALG_EDIT_GRAPH_HPP_ */
