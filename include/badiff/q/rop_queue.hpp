#ifndef BADIFF_Q_MINIMIZE_ROP_QUEUE_HPP_
#define BADIFF_Q_MINIMIZE_ROP_QUEUE_HPP_

#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

class ROpQueue : public FilterOpQueue {
public:
  ROpQueue(std::unique_ptr<OpQueue> source);
  virtual ~ROpQueue();

protected:
  virtual bool Pull() override;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_CHUNKING_OP_QUEUE_HPP_ */
