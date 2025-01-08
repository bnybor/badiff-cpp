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

#include <badiff/q/chunking_op_queue.hpp>

#include <algorithm>

namespace badiff {
namespace q {

ChunkingOpQueue::ChunkingOpQueue(std::unique_ptr<OpQueue> source, int chunk_len)
    : FilterOpQueue(std::move(source)), chunk_len_(chunk_len) {}

ChunkingOpQueue::~ChunkingOpQueue() {}

bool ChunkingOpQueue::FlushInterleaved(unsigned int n) {
  while (n-- > 0 && (!deletes_.empty() || !inserts_.empty())) {
    if (!deletes_.empty()) {
      Prepare(std::move(deletes_.front()));
      deletes_.erase(deletes_.begin());
    }
    if (!inserts_.empty()) {
      Prepare(std::move(inserts_.front()));
      inserts_.erase(inserts_.begin());
    }
  }
  return Flush();
}

bool ChunkingOpQueue::Pull() {
  if (!Require(1))
    return FlushInterleaved(-1);

  auto op(std::move(filtering_.front()));
  filtering_.erase(filtering_.begin());

  if (op.GetType() == Op::NEXT) {
    FlushInterleaved(-1);
    Prepare(std::move(op));
    return true;
  }

  for (int i = 0; i < op.GetLength(); i += chunk_len_) {
    int extent = std::min(op.GetLength() - i, chunk_len_);
    std::unique_ptr<char[]> value(new char[extent]);
    std::copy(op.GetValue().get() + i, op.GetValue().get() + i + extent,
              value.get());
    Op chunk(op.GetType(), extent, std::move(value));
    if (chunk.GetType() == Op::DELETE) {
      deletes_.push_back(std::move(chunk));
    } else {
      inserts_.push_back(std::move(chunk));
    }
  }

  return FlushInterleaved(1);
}

} // namespace q
} // namespace badiff
