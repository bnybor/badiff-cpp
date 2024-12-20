#ifndef BADIFF_ALG_GRAPH_HPP_
#define BADIFF_ALG_GRAPH_HPP_

#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace alg {

class Graph {
public:
  virtual ~Graph() = default;

  /**
   * \brief Compute an OpQueue that transforms `original` into `target`.
   */
  virtual void Compute(const char *original, std::size_t original_length,
                       const char *target, std::size_t target_length) = 0;

  /**
   * \brief Return a copy of the OpQueue computed by `Compute`.
   */
  virtual std::unique_ptr<q::OpQueue> MakeOpQueue() const = 0;
};

} // namespace alg
} // namespace badiff

#endif /* BADIFF_ALG_GRAPH_HPP_ */
