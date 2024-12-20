#ifndef BADIFF_ALG_INERTIAL_GRAPH_HPP_
#define BADIFF_ALG_INERTIAL_GRAPH_HPP_

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

  void Compute(const char *original, std::size_t original_length,
               const char *target, std::size_t target_length) override;

  virtual std::unique_ptr<q::OpQueue> MakeOpQueue() const override;

  std::vector<int> cost_;
  std::vector<char> xval_;
  std::vector<char> yval_;
};

} // namespace alg

} // namespace badiff

#endif /* BADIFF_ALG_EDIT_GRAPH_HPP_ */
