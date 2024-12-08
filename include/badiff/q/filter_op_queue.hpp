#ifndef BADIFF_Q_FILTER_OP_QUEUE_HPP_
#define BADIFF_Q_FILTER_OP_QUEUE_HPP_

#include <badiff/q/op_queue.hpp>
#include <memory>

namespace badiff {
namespace q {

class FilterOpQueue : public OpQueue {
public:
  FilterOpQueue(std::unique_ptr<OpQueue> source);

  virtual void Push(Op op) override;

protected:
  std::unique_ptr<OpQueue> source_;
  std::vector<Op> filtering_;

  bool Require(std::size_t count);
  void Drop(std::size_t count);
  bool Flush();
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_FILTER_OP_QUEUE_HPP_ */
