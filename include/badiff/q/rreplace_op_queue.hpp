#ifndef BADIFF_Q_REPLACE_OP_QUEUE_HPP_
#define BADIFF_Q_REPLACE_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>

#include <badiff/defaults.hpp>

namespace badiff {
namespace q {

class RReplaceOpQueue : public OpQueue {
public:
  RReplaceOpQueue(const char *original, int original_len, const char *target,
                  int target_len, int max_chunk_len = DEFAULT_CHUNK);
  virtual ~RReplaceOpQueue();

protected:
  bool Pull() override;

  const char *original_;
  const char *target_;

  int original_pos_, original_len_;
  int target_pos_, target_len_;

  int original_chunk_len_;
  int target_chunk_len_;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */
