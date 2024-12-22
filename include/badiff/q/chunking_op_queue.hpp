#ifndef BADIFF_Q_CHUNKING_OP_QUEUE_HPP_
#define BADIFF_Q_CHUNKING_OP_QUEUE_HPP_

#include <badiff/q/filter_op_queue.hpp>

#include <badiff/defaults.hpp>

namespace badiff {
namespace q {

class ChunkingOpQueue : public FilterOpQueue {
public:
  ChunkingOpQueue(std::unique_ptr<OpQueue> source,
                  int chunk_size = DEFAULT_CHUNK);
  virtual ~ChunkingOpQueue();

protected:
  virtual bool Pull() override;

  int chunk_size_;
  std::vector<Op> deletes_;
  std::vector<Op> inserts_;

  bool FlushInterleaved(unsigned int n);
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_CHUNKING_OP_QUEUE_HPP_ */
