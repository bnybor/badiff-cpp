#ifndef BADIFF_Q_GRAPH_OP_QUEUE_HPP_
#define BADIFF_Q_GRAPH_OP_QUEUE_HPP_

#include <badiff/alg/graph.hpp>
#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

class GraphOpQueue : public FilterOpQueue {
public:
  enum Pairing {
    INSERT_THEN_DELETE,
    DELETE_THEN_INSERT,
    ANY,
  };

  GraphOpQueue(std::unique_ptr<OpQueue> source,
               std::unique_ptr<alg::Graph> graph, Pairing pairing = ANY);
  virtual ~GraphOpQueue();

protected:
  std::unique_ptr<alg::Graph> graph_;
  Pairing pairing_;

  virtual bool Pull() override;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_GRAPH_OP_QUEUE_HPP_ */
