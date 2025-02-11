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

#ifndef BADIFF_Q_STREAM_REPLACE_OP_QUEUE_HPP_
#define BADIFF_Q_STREAM_REPLACE_OP_QUEUE_HPP_

#include <functional>

#include <badiff/q/op_queue.hpp>

#include <badiff/defaults.hpp>

namespace badiff {
namespace q {

class StreamReplaceOpQueue : public OpQueue {
public:
  StreamReplaceOpQueue(
      std::istream &original, int original_len, std::istream &target,
      int target_len, int max_chunk_len = DEFAULT_CHUNK,
      std::function<void(int original_pos, int target_pos, int original_len,
                         int target_len)> *reporter = nullptr);
  virtual ~StreamReplaceOpQueue();

protected:
  virtual bool Pull() override;

  std::function<void(int original_pos, int target_pos, int original_len,
                     int target_len)> *reporter_;

  std::istream &original_;
  std::istream &target_;
  int original_chunk_len_;
  int target_chunk_len_;
  int original_pos_;
  int target_pos_;
  int original_len_;
  int target_len_;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */
