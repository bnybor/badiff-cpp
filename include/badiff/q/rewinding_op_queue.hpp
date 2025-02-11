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

#ifndef BADIFF_Q_REWINDING_OP_QUEUE_HPP_
#define BADIFF_Q_REWINDING_OP_QUEUE_HPP_

#include <badiff/q/filter_op_queue.hpp>

#include <map>

namespace badiff {
namespace q {

class RewindingOpQueue : public FilterOpQueue {
public:
  RewindingOpQueue(std::unique_ptr<OpQueue> op_queue);

protected:
  std::vector<Op> history_;

  bool FlushHistory();
  virtual bool Pull() override;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REWINDING_OP_QUEUE_HPP_ */
