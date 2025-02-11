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

#include <badiff/q/replace_op_queue.hpp>

namespace badiff {
namespace q {

ReplaceOpQueue::ReplaceOpQueue(
    const char *original, int original_size, const char *target,
    int target_size, int max_chunk_size,
    std::function<void(int original_pos, int target_pos)> *reporter)
    : original_(original), target_(target), original_pos_(0),
      original_len_(original_size), target_pos_(0), target_len_(target_size) {
  reporter_ = reporter;
  int chunks =
      std::max(1, std::max(original_size, target_size) / max_chunk_size);
  original_chunk_len_ = std::max(1, original_size / chunks);
  target_chunk_len_ = std::max(1, target_size / chunks);
}

ReplaceOpQueue::~ReplaceOpQueue() {}

bool ReplaceOpQueue::Pull() {
  bool prepared = false;
  if (original_pos_ < original_len_) {
    int size = std::min(original_chunk_len_, original_len_ - original_pos_);
    std::unique_ptr<char[]> value(new char[size]);
    std::copy(original_ + original_pos_, original_ + original_pos_ + size,
              value.get());
    Prepare(Op(Op::DELETE, size, std::move(value)));
    original_pos_ += size;
    prepared = true;
  }
  if (target_pos_ < target_len_) {
    int size = std::min(target_chunk_len_, target_len_ - target_pos_);
    std::unique_ptr<char[]> value(new char[size]);
    std::copy(target_ + target_pos_, target_ + target_pos_ + size, value.get());
    Prepare(Op(Op::INSERT, size, std::move(value)));
    target_pos_ += size;
    prepared = true;
  }
  if (prepared && reporter_) {
    (*reporter_)(original_pos_, target_pos_);
  }
  return prepared;
}

} // namespace q
} // namespace badiff
