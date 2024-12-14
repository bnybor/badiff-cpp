#ifndef BADIFF_Q_STREAM_REPLACE_OP_QUEUE_HPP_
#define BADIFF_Q_STREAM_REPLACE_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

class StreamReplaceOpQueue : public OpQueue {
public:
  StreamReplaceOpQueue(std::istream &original, std::istream &target,
                       int chunk_size);
  virtual ~StreamReplaceOpQueue();

protected:
  virtual bool Pull() override;

  std::istream &original_;
  std::istream &target_;
  int chunk_size_;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */