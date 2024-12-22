#ifndef BADIFF_Q_REPLACE_OP_QUEUE_HPP_
#define BADIFF_Q_REPLACE_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>

#include <badiff/defaults.hpp>

namespace badiff {
namespace q {

class ReplaceOpQueue : public OpQueue {
public:
  ReplaceOpQueue(const char *original, int original_size, const char *target,
                 int target_size, int max_chunk_size = DEFAULT_CHUNK);
  virtual ~ReplaceOpQueue();

protected:
  bool Pull() override;

  const char *original_;
  const char *target_;

  int original_pos_, original_size_;
  int target_pos_, target_size_;

  int original_chunk_size_;
  int target_chunk_size_;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */
