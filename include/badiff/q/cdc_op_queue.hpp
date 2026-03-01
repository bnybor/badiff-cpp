/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef BADIFF_Q_CDC_OP_QUEUE_HPP_
#define BADIFF_Q_CDC_OP_QUEUE_HPP_

#include <badiff/defaults.hpp>
#include <badiff/q/op_queue.hpp>

#include <cstdint>
#include <deque>
#include <functional>

namespace badiff {
namespace q {

/**
 * Content-defined chunking source queue. Splits the original and target byte
 * arrays at content-defined boundaries using a gear hash rolling window, then
 * matches identical chunks (by exact content) as NEXT ops and interleaves
 * unmatched DELETE/INSERT pairs for downstream graph diffing.
 *
 * Unlike ReplaceOpQueue, this finds matching blocks regardless of their
 * position in the file, so moved or shifted content is detected correctly.
 */
class CdcOpQueue : public OpQueue {
public:
  CdcOpQueue(const char *original, int original_len, const char *target,
             int target_len, int avg_chunk = DEFAULT_CHUNK,
             std::function<void(int, int, int, int)> *reporter = nullptr);
  virtual ~CdcOpQueue();

protected:
  virtual bool Pull() override;

private:
  int min_chunk_;
  int max_chunk_;
  uint32_t mask_;

  std::function<void(int, int, int, int)> *reporter_;
  int original_len_, target_len_;
  int original_pos_, target_pos_;

  std::vector<Op> deletes_;
  std::vector<Op> inserts_;
  std::deque<Op> output_;

  void CdcSplit(const char *data, int len, Op::Type type,
                std::vector<Op> &out);
  uint32_t ChunkHash(const char *data, int len) const;
  void Plan();
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_CDC_OP_QUEUE_HPP_ */
