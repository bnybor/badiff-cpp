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

#include <badiff/alg/inertial_graph.hpp>
#include <badiff/q/compacting_op_queue.hpp>
#include <badiff/q/op_queue.hpp>
#include <badiff/q/vector_op_queue.hpp>

#include <climits>

namespace badiff {

namespace alg {

namespace {
static int DEFAULT_TRANSITION_COSTS[4][4] = {
    {1, 1, 1, 1}, // From STOP to...
    {3, 1, 3, 4}, // From DELETE to...
    {2, 2, 1, 3}, // From INSERT to...
    {1, 2, 3, 1}, // From NEXT to...
                  //           S	D	I	N
};

int Cost(Op::Type from, Op::Type to) {
  return DEFAULT_TRANSITION_COSTS[from][to];
}

/**
 * Saturating cost addition. Entry costs use INT_MAX as "infinity" for an
 * unavailable path, which stays infinite.
 */
static inline int AddCost(int a, int b) {
  if (a == INT_MAX || b == INT_MAX)
    return INT_MAX;
  return a + b;
}

static char DELETE = 0;
static char INSERT = 1;
static char NEXT = 2;

static int NUM_FIELDS = 3;
} // namespace

void InertialGraph::Compute(const char *original, std::size_t original_length,
                            const char *target, std::size_t target_length) {
  cost_ = std::vector<int>(NUM_FIELDS * (original_length + 1) *
                           (target_length + 1));

  xval_.resize(original_length + 1);
  yval_.resize(target_length + 1);

  std::copy(original, original + original_length, xval_.begin() + 1);
  std::copy(target, target + target_length, yval_.begin() + 1);

  cost_[DELETE] = 0;
  cost_[INSERT] = 0;
  cost_[NEXT] = 0;

  int cdd, cdi, cdn, cid, cii, cin, cnd, cni, cnn;
  cdd = Cost(Op::DELETE, Op::DELETE);
  cdi = Cost(Op::DELETE, Op::INSERT);
  cdn = Cost(Op::DELETE, Op::NEXT);
  cid = Cost(Op::INSERT, Op::DELETE);
  cii = Cost(Op::INSERT, Op::INSERT);
  cin = Cost(Op::INSERT, Op::NEXT);
  cnd = Cost(Op::NEXT, Op::DELETE);
  cni = Cost(Op::NEXT, Op::INSERT);
  cnn = Cost(Op::NEXT, Op::NEXT);

  int costLength = NUM_FIELDS * (original_length + 1) * (target_length + 1);
  int xvalLength = original_length + 1;
  int yvalLength = target_length + 1;

  int pmax = xvalLength * yvalLength;

  int x = 0;
  int y = 0;
  for (int pos = 0; pos < pmax; pos++) {
    // mark entry costs
    int edc, eic, enc, f;

    f = (pos - 1) * NUM_FIELDS + DELETE;
    f = (f + costLength) % costLength;
    edc = (x == 0) ? ((y == 0) ? 0 : INT_MAX) : cost_[f];

    f = (pos - xvalLength) * NUM_FIELDS + INSERT;
    f = (f + costLength) % costLength;
    eic = (y == 0) ? ((x == 0) ? 0 : INT_MAX) : cost_[f];

    f = (pos - 1 - xvalLength) * NUM_FIELDS + NEXT;
    f = (f + costLength) % costLength;
    enc = (x == 0) ? ((y == 0) ? 0 : INT_MAX)
          : (y == 0) ? INT_MAX
                     : ((xval_[x] == yval_[y]) ? cost_[f] : INT_MAX);

    int cost, c;

    // compute delete cost
    cost = AddCost(edc, cdd);
    c = AddCost(eic, cid);
    cost = c < cost ? c : cost;
    c = AddCost(enc, cnd);
    cost = c < cost ? c : cost;
    cost_[pos * NUM_FIELDS + DELETE] = cost;

    // compute insert cost
    cost = AddCost(eic, cii);
    c = AddCost(edc, cdi);
    cost = c < cost ? c : cost;
    c = AddCost(enc, cni);
    cost = c < cost ? c : cost;
    cost_[pos * NUM_FIELDS + INSERT] = cost;

    // compute next cost
    cost = AddCost(enc, cnn);
    c = AddCost(edc, cdn);
    cost = c < cost ? c : cost;
    c = AddCost(eic, cin);
    cost = c < cost ? c : cost;
    cost_[pos * NUM_FIELDS + NEXT] = cost;

    x++;
    y += (x / xvalLength);
    x %= xvalLength;
  }
}

class InertialGraphOpQueue : public q::OpQueue {
  const InertialGraph *graph_;
  int pos;
  Op::Type prev;
  int x;
  int y;

public:
  InertialGraphOpQueue(const InertialGraph *graph) : graph_(graph) {
    pos = graph_->xval_.size() * graph_->yval_.size() - 1;
    x = graph_->xval_.size() - 1;
    y = graph_->yval_.size() - 1;
    prev = Op::STOP;
  }

  bool Pull() {
    if (pos == 0)
      return false;

    Op::Type op = Op::STOP;
    int cost = INT_MAX;
    if (x > 0 && y > 0 && graph_->xval_[x] == graph_->yval_[y]) {
      op = Op::NEXT;
      cost = AddCost(
          graph_->cost_[(pos - 1 - graph_->xval_.size()) * NUM_FIELDS + NEXT],
          Cost(Op::NEXT, prev));
    }

    if (y > 0) {
      int insert_cost = AddCost(
          graph_->cost_[(pos - graph_->xval_.size()) * NUM_FIELDS + INSERT],
          Cost(Op::INSERT, prev));
      if (x == 0 || insert_cost < cost) {
        op = Op::INSERT;
        cost = insert_cost;
      }
    }

    if (x > 0) {
      int delete_cost = AddCost(graph_->cost_[(pos - 1) * NUM_FIELDS + DELETE],
                                Cost(Op::DELETE, prev));
      if (y == 0 || delete_cost < cost) {
        op = Op::DELETE;
        cost = delete_cost;
      }
    }

    Op e;

    switch (op) {
    case Op::NEXT: {
      e = Op(Op::NEXT, 1);
      pos = pos - 1 - graph_->xval_.size();
      x--;
      y--;
      break;
    }
    case Op::INSERT: {
      std::unique_ptr<char[]> value(new char[1]);
      value[0] = graph_->yval_[y];
      e = Op(Op::INSERT, 1, std::move(value));
      pos = pos - graph_->xval_.size();
      y--;
      break;
    }
    case Op::DELETE: {
      std::unique_ptr<char[]> value(new char[1]);
      value[0] = graph_->xval_[x];
      e = Op(Op::DELETE, 1, std::move(value));
      pos = pos - 1;
      x--;
      break;
    }
    }

    Prepare(e);

    prev = e.GetType();

    return true;
  }
};

std::unique_ptr<q::OpQueue> InertialGraph::MakeOpQueue() const {
  std::unique_ptr<q::OpQueue> rq(new InertialGraphOpQueue(this));
  std::vector<Op> ops;
  while (!rq->IsEmpty()) {
    ops.insert(ops.begin(), *rq->Pop());
  }

  std::unique_ptr<q::OpQueue> q(new q::VectorOpQueue(ops));
  q.reset(new q::CompactingOpQueue(std::move(q)));
  return q;
}

} // namespace alg
} // namespace badiff
