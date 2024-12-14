#ifndef BADIFF_Q_REPLACE_OP_QUEUE_HPP_
#define BADIFF_Q_REPLACE_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

class ReplaceOpQueue : public OpQueue {
public:
  ReplaceOpQueue(const char *original, std::size_t original_size,
                 const char *target, std::size_t target_size);
  virtual ~ReplaceOpQueue();
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */
