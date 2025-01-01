#ifndef BADIFF_Q_RSTREAM_REPLACE_OP_QUEUE_HPP_
#define BADIFF_Q_RSTREAM_REPLACE_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>

#include <badiff/defaults.hpp>

namespace badiff {
namespace q {

class RStreamReplaceOpQueue : public OpQueue {
public:
  RStreamReplaceOpQueue(std::istream &original, int original_len,
                        std::istream &target, int target_len,
                        int max_chunk_len = DEFAULT_CHUNK);
  virtual ~RStreamReplaceOpQueue();

protected:
  virtual bool Pull() override;

  std::istream &original_;
  std::istream &target_;
  int original_chunk_len_;
  int target_chunk_len_;
  int original_pos_;
  int target_pos_;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */
