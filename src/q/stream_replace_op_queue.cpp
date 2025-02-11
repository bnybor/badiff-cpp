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

#include <badiff/q/stream_replace_op_queue.hpp>

namespace badiff {
namespace q {

StreamReplaceOpQueue::StreamReplaceOpQueue(
    std::istream &original, int original_len, std::istream &target,
    int target_len, int max_chunk_len,
    std::function<void(int original_pos, int target_pos)> *reporter)
    : original_(original), target_(target), original_len_(original_len),
      target_len_(target_len) {
  reporter_ = reporter;
  int chunks = std::max(1, std::max(original_len, target_len) / max_chunk_len);
  original_chunk_len_ = std::max(1, original_len / chunks);
  target_chunk_len_ = std::max(1, target_len / chunks);
  original_pos_ = 0;
  target_pos_ = 0;
}

StreamReplaceOpQueue::~StreamReplaceOpQueue() {}

bool StreamReplaceOpQueue::Pull() {
  bool prepared = false;

  if (original_pos_ < original_len_) {
    std::unique_ptr<char[]> value(new char[original_chunk_len_]);
    int n = 0;
    do {
      original_.seekg(original_pos_++);
      value[n++] = original_.get();
    } while (n < original_chunk_len_ && original_pos_ < original_len_);
    Prepare(Op(Op::DELETE, n, std::move(value)));
    prepared = true;
  }
  if (target_pos_ < target_len_) {
    std::unique_ptr<char[]> value(new char[target_chunk_len_]);
    int n = 0;
    do {
      target_.seekg(target_pos_++);
      value[n++] = target_.get();
    } while (n < target_chunk_len_ && target_pos_ < target_len_);
    Prepare(Op(Op::INSERT, n, std::move(value)));
    prepared = true;
  }
  if (prepared && reporter_) {
    (*reporter_)(original_pos_, target_pos_);
  }
  return prepared;
}

} // namespace q
} // namespace badiff
