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

#include <algorithm>
#include <badiff/alg/edit_graph.hpp>
#include <badiff/q/vector_op_queue.hpp>

namespace badiff {

namespace alg {

void EditGraph::Compute(const char *original, std::size_t original_length,
                        const char *target, std::size_t target_length) {
  /*
   * Construct a rectangular graph as the edit graph.
   * The edit graph supports down (insert), right (delete), and
   * diagonal down-right (next) traversal.  The edit graph
   * is a digraph without cycles and can be traversed in
   * O(n^2) complexity and memory.
   */

  std::size_t xlen = original_length + 1;
  std::size_t ylen = target_length + 1;

  std::unique_ptr<char[]> xval(new char[xlen]);
  std::unique_ptr<char[]> yval(new char[ylen]);

  xval[0] = 0;
  yval[0] = 0;
  std::copy(original, original + original_length, &xval[1]);
  std::copy(target, target + target_length, &yval[1]);

  // The best operation to enter a comparison.
  Op::Type best_op[xlen * ylen];
  // The path length of the best operation to enter a comparison.
  std::size_t best_length[xlen * ylen];

  std::fill(best_op, best_op + xlen * ylen, Op::STOP);
  std::fill(best_length, best_length + xlen * ylen, 0);

  /*
   * Actually traverse the graph, using dynamic programming to store
   * the best path.
   */

  for (std::size_t y = 0; y < target_length + 1; ++y) {
    for (std::size_t x = 0; x < original_length + 1; ++x) {
      if (x == 0 && y == 0)
        continue;
      std::size_t pos = x + y * xlen;
      if (x > 0 && y > 0 && xval[x] == yval[y]) {
        best_op[pos] = Op::NEXT;
        best_length[pos] = (1 + best_length[pos - xlen - 1]);
        continue;
      }
      std::size_t dlen = (x > 0) ? (1 + best_length[pos - 1]) : -1;
      std::size_t ilen = (y > 0) ? (1 + best_length[pos - xlen]) : -1;
      if (dlen <= ilen) {
        best_op[pos] = Op::DELETE;
        best_length[pos] = dlen;
      } else {
        best_op[pos] = Op::INSERT;
        best_length[pos] = ilen;
      }
    }
  }

  std::vector<Op> ret;
  std::vector<char> buf;
  Op::Type op = Op::STOP;
  std::size_t run = 0;

  /*
   * Traverse backward from lower right corner to upper left corner.
   * This gives the edit script in reverse order.
   */

  int pos = xlen * ylen - 1;
  while (pos > 0) {
    Op::Type fop = best_op[pos];
    if (op != Op::STOP && op != fop) {
      std::unique_ptr<char[]> data;
      if (op == Op::INSERT || op == Op::DELETE) {
        auto *rdata = &buf[0];
        data.reset(new char[buf.size()]);
        for (std::size_t i = 0; i < buf.size(); ++i) {
          data[buf.size() - i - 1] = rdata[i];
        }
      }
      ret.push_back(
          Op(op, run, std::move(data))); // @suppress("Ambiguous problem")
      run = 0;
      buf.clear();
    }
    op = fop;
    run++;
    if (op == Op::INSERT) {
      buf.push_back(yval[pos / xlen]);
      pos -= xlen;
    }
    if (op == Op::DELETE) {
      buf.push_back(xval[pos % xlen]);
      pos -= 1;
    }
    if (op == Op::NEXT) {
      pos -= xlen + 1;
    }
  }
  if (op != Op::STOP) {
    std::unique_ptr<char[]> data;
    if (op == Op::INSERT || op == Op::DELETE) {
      auto *rdata = &buf[0];
      data.reset(new char[buf.size()]);
      for (std::size_t i = 0; i < buf.size(); ++i) {
        data[buf.size() - i - 1] = rdata[i];
      }
    }
    ret.push_back(
        Op(op, run, std::move(data))); // @suppress("Ambiguous problem")
  }

  /*
   * Finally, reverse the edit script and store it.
   */
  op_queue_.resize(ret.size());
  std::move(ret.rbegin(), ret.rend(), op_queue_.begin());
}

std::unique_ptr<q::OpQueue> EditGraph::MakeOpQueue() const {
  /*
   * Copy the edit script and return it as an OpQueue.
   */
  std::vector<Op> queue;
  for (const auto &op : op_queue_) {
    std::unique_ptr<char[]> value;
    if (op.GetValue()) {
      value.reset(new char[op.GetLength()]);
      std::copy(op.GetValue().get(), op.GetValue().get() + op.GetLength(),
                value.get());
    }
    queue.push_back(Op(op.GetType(), op.GetLength(),
                       std::move(value))); // @suppress("Ambiguous problem")
  }
  return std::unique_ptr<q::OpQueue>(new q::VectorOpQueue(std::move(queue)));
}

} // namespace alg

} // namespace badiff
