#ifndef Q_VECTOR_OP_QUEUE_HPP_
#define Q_VECTOR_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

class VectorOpQueue : public OpQueue {
public:
  VectorOpQueue() = default;
  VectorOpQueue(std::vector<Op> vec);
  virtual ~VectorOpQueue();
};

} // namespace q
} // namespace badiff

#endif /* Q_VECTOR_OP_QUEUE_HPP_ */
