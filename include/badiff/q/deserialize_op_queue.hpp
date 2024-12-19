#ifndef BADIFF_Q_DESERIALIZE_OP_QUEUE_HPP_
#define BADIFF_Q_DESERIALIZE_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>

#include <istream>

namespace badiff {
namespace q {

class DeserializeOpQueue : public OpQueue {
public:
  DeserializeOpQueue(std::istream &delta);

protected:
  bool Pull() override;

  bool has_next_;
  std::istream &delta_;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_DESERIALIZE_OP_QUEUE_HPP_ */
