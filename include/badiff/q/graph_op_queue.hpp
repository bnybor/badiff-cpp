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

#ifndef BADIFF_Q_GRAPH_OP_QUEUE_HPP_
#define BADIFF_Q_GRAPH_OP_QUEUE_HPP_

#include <badiff/alg/graph.hpp>
#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

class GraphOpQueue : public FilterOpQueue {
public:
  enum Pairing {
    INSERT_THEN_DELETE,
    DELETE_THEN_INSERT,
    ANY,
  };

  GraphOpQueue(std::unique_ptr<OpQueue> source,
               std::unique_ptr<alg::Graph> graph, Pairing pairing = ANY);
  virtual ~GraphOpQueue();

protected:
  std::unique_ptr<alg::Graph> graph_;
  Pairing pairing_;

  virtual bool Pull() override;
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_GRAPH_OP_QUEUE_HPP_ */
