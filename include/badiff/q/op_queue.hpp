/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

protected:
  virtual bool Pull();
  virtual void Prepare(Op op);

public:
  static std::string SummarizeConsuming(OpQueue &op_queue);
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_OP_QUEUE_HPP_ */
