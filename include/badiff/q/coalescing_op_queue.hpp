#ifndef BADIFF_Q_COALESCING_OP_QUEUE_HPP_
#define BADIFF_Q_COALESCING_OP_QUEUE_HPP_

#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

class CoalescingOpQueue : public FilterOpQueue {
public:
  CoalescingOpQueue(std::unique_ptr<OpQueue> source);
  virtual ~CoalescingOpQueue();

protected:
  virtual bool Pull() override;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */
