#ifndef BADIFF_Q_OP_QUEUE_HPP_
#define BADIFF_Q_OP_QUEUE_HPP_

#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace badiff {
namespace q {
class OpQueue;
}
} // namespace badiff

#include <badiff/op.hpp>

namespace badiff {
namespace q {

class OpQueue {
protected:
  std::vector<Op> queue_;

public:
  virtual ~OpQueue() = default;

  OpQueue() = default;
  explicit OpQueue(const OpQueue &);
  OpQueue(OpQueue &&) = default;
  OpQueue &operator=(const OpQueue &) = delete;
  OpQueue &operator=(OpQueue &&) = default;

  OpQueue(std::vector<Op>);

  virtual bool IsEmpty();
  virtual Op PopFront();
  virtual void PushBack(Op);
  virtual void PushBack(OpQueue &);

  static std::string SummarizeConsuming(OpQueue &op_queue);

protected:
  virtual void Pull();
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_OP_QUEUE_HPP_ */
