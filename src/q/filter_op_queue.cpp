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

#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

FilterOpQueue::FilterOpQueue(std::unique_ptr<OpQueue> source)
    : source_(std::move(source)) {}

FilterOpQueue::~FilterOpQueue() {}

void FilterOpQueue::Push(Op op) { source_->Push(std::move(op)); }

bool FilterOpQueue::Require(std::size_t count) {
  while (filtering_.size() < count) {
    if (source_->IsEmpty())
      return false;
    filtering_.push_back(*source_->Pop());
  }
  return true;
}

void FilterOpQueue::Drop(std::size_t count) {
  filtering_.erase(filtering_.begin(), filtering_.begin() + count);
}

bool FilterOpQueue::Flush(std::size_t n) {
  while (!filtering_.empty()) {
    auto &e = filtering_.front();
    if (n-- == 0)
      return true;
    Prepare(std::move(e));
    filtering_.erase(filtering_.begin());
  }
  return true;
}

} // namespace q
} // namespace badiff
