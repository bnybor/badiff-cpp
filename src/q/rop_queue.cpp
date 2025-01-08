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

#include <badiff/q/rop_queue.hpp>

#include <algorithm>

namespace badiff {
namespace q {

ROpQueue::ROpQueue(std::unique_ptr<OpQueue> source)
    : FilterOpQueue(std::move(source)) {}

ROpQueue::~ROpQueue() {}

bool ROpQueue::Pull() {
  if (!Require(1))
    return false;

  auto op(std::move(filtering_.front()));
  filtering_.erase(filtering_.begin());

  if (op.GetValue()) {
    std::unique_ptr<char[]> reverse(new char[op.GetLength()]);
    for (int i = 0; i < op.GetLength(); i++) {
      reverse[op.GetLength() - 1 - i] = op.GetValue()[i];
    }
    op.MutableValue() = std::move(reverse);
  }

  Prepare(std::move(op));

  return true;
}

} // namespace q
} // namespace badiff
