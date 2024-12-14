#ifndef BADIFF_Q_GRAPH_OP_QUEUE_HPP_
#define BADIFF_Q_GRAPH_OP_QUEUE_HPP_

#include <badiff/alg/graph.hpp>
#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

class GraphOpQueue : public FilterOpQueue {
public:
  GraphOpQueue(std::unique_ptr<OpQueue> source,
               std::unique_ptr<alg::Graph> graph);
  virtual ~GraphOpQueue();

protected:
  std::unique_ptr<alg::Graph> graph_;

  virtual bool Pull() override;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_GRAPH_OP_QUEUE_HPP_ */
