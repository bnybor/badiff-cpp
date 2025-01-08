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

#ifndef BADIFF_Q_CHUNKING_OP_QUEUE_HPP_
#define BADIFF_Q_CHUNKING_OP_QUEUE_HPP_

#include <badiff/q/filter_op_queue.hpp>

#include <badiff/defaults.hpp>

namespace badiff {
namespace q {

class ChunkingOpQueue : public FilterOpQueue {
public:
  ChunkingOpQueue(std::unique_ptr<OpQueue> source,
                  int chunk_len = DEFAULT_CHUNK);
  virtual ~ChunkingOpQueue();

protected:
  virtual bool Pull() override;

  int chunk_len_;
  std::vector<Op> deletes_;
  std::vector<Op> inserts_;

  bool FlushInterleaved(unsigned int n);
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_CHUNKING_OP_QUEUE_HPP_ */
