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
  std::vector<Op> prepared_;

public:
  virtual ~OpQueue();

  OpQueue() = default;
  explicit OpQueue(const OpQueue &);
  OpQueue(OpQueue &&) = default;
  OpQueue &operator=(const OpQueue &) = delete;
  OpQueue &operator=(OpQueue &&) = default;

  virtual std::unique_ptr<Op> Pop();
  virtual void Push(Op op);
  virtual bool IsEmpty();

  void Serialize(std::ostream &out);
  void Deserialize(std::istream &in);
  void Apply(std::istream &original, std::ostream &target);

  virtual std::size_t PreparedSize() const;

protected:
  virtual bool Pull();
  virtual void Prepare(Op op);

public:
  static std::string SummarizeConsuming(OpQueue &op_queue);
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_OP_QUEUE_HPP_ */
