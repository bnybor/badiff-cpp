#ifndef BADIFF_ALG_EDIT_GRAPH_HPP_
#define BADIFF_ALG_EDIT_GRAPH_HPP_

#include <vector>

#include <badiff/alg/graph.hpp>
#include <badiff/op.hpp>

namespace badiff {
namespace alg {

class InertialGraph : public Graph {
  std::vector<Op> op_queue_;

public:
  static std::size_t TRANSITION_COSTS[4][4];
  virtual ~InertialGraph() = default;

  void Compute(const Byte *original, std::size_t original_length,
               const Byte *target, std::size_t target_length) override;

  virtual std::unique_ptr<q::OpQueue> MakeOpQueue() const override;
};

} // namespace alg

} // namespace badiff

#endif /* BADIFF_ALG_EDIT_GRAPH_HPP_ */
