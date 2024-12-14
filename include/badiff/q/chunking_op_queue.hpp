#ifndef BADIFF_Q_CHUNKING_OP_QUEUE_HPP_
#define BADIFF_Q_CHUNKING_OP_QUEUE_HPP_

#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

class ChunkingOpQueue : public FilterOpQueue {
public:
  ChunkingOpQueue(std::unique_ptr<OpQueue> source, int chunk_size);
  virtual ~ChunkingOpQueue();

protected:
  virtual bool Pull() override;

  int chunk_size_;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_CHUNKING_OP_QUEUE_HPP_ */
