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

#ifndef BADIFF_ALG_INERTIAL_GRAPH_HPP_
#define BADIFF_ALG_INERTIAL_GRAPH_HPP_

#include <vector>

#include <badiff/alg/graph.hpp>
#include <badiff/op.hpp>

namespace badiff {
namespace alg {

class InertialGraph : public Graph {
  std::vector<Op> op_queue_;

public:
  static std::size_t TRANSITION_COSTS[4][4];
  virtual ~InertialGraph() = default;

  void Compute(const char *original, std::size_t original_length,
               const char *target, std::size_t target_length) override;

  virtual std::unique_ptr<q::OpQueue> MakeOpQueue() const override;

  std::vector<int> cost_;
  std::vector<char> xval_;
  std::vector<char> yval_;
};

} // namespace alg

} // namespace badiff

#endif /* BADIFF_ALG_EDIT_GRAPH_HPP_ */
