#ifndef BADIFF_Q_MINIMIZE_OP_QUEUE_HPP_
#define BADIFF_Q_MINIMIZE_OP_QUEUE_HPP_

#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

class MinimizeOpQueue : public FilterOpQueue {
public:
  MinimizeOpQueue(std::unique_ptr<OpQueue> source);
  virtual ~MinimizeOpQueue();

protected:
  virtual bool Pull() override;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_CHUNKING_OP_QUEUE_HPP_ */
