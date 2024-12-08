#include <algorithm>
#include <badiff/alg/inertial_graph.hpp>
#include <badiff/q/op_queue.hpp>
#include <badiff/q/vector_op_queue.hpp>
#include <iostream>
#include <numeric>

namespace badiff {

namespace alg {

std::size_t InertialGraph::TRANSITION_COSTS[4][4] =  //
    {
        //
        {
            1,
            1,
            1,
            1,
        },  // From STOP
        {
            3,
            1,
            3,
            4,
        },  // From DELETE
        {
            2,
            2,
            1,
            3,
        },  // From INSERT
        {
            1,
            2,
            3,
            1,
        }  // From NEXT
};  // To S D I N

namespace {
inline std::size_t CostOfOperation(Op::Type previous_op, Op::Type op) {
  return InertialGraph::TRANSITION_COSTS[(std::size_t)previous_op]
                                        [(std::size_t)op];
}
struct Node {
  const std::size_t *previous_delete_cost_;
  const std::size_t *previous_insert_cost_;
  const std::size_t *previous_next_cost_;
  std::size_t delete_cost_;
  std::size_t insert_cost_;
  std::size_t next_cost_;

  std::uint8_t original_;
  std::uint8_t target_;

  Node() {
    previous_delete_cost_ = nullptr;
    previous_insert_cost_ = nullptr;
    previous_next_cost_ = nullptr;
    delete_cost_ = (std::uint32_t)-1;
    insert_cost_ = (std::uint32_t)-1;
    next_cost_ = (std::uint32_t)-1;
    original_ = 0;
    target_ = 0;
  }

  void Compute() {
    if (previous_delete_cost_) {
      delete_cost_ =
          std::min(delete_cost_, *previous_delete_cost_ +
                                     CostOfOperation(Op::DELETE, Op::DELETE));
      insert_cost_ =
          std::min(insert_cost_, *previous_delete_cost_ +
                                     CostOfOperation(Op::DELETE, Op::INSERT));
      if (original_ == target_)
        next_cost_ =
            std::min(next_cost_, *previous_delete_cost_ +
                                     CostOfOperation(Op::DELETE, Op::NEXT));
    }
    if (previous_insert_cost_) {
      delete_cost_ =
          std::min(delete_cost_, *previous_insert_cost_ +
                                     CostOfOperation(Op::INSERT, Op::DELETE));
      insert_cost_ =
          std::min(insert_cost_, *previous_insert_cost_ +
                                     CostOfOperation(Op::INSERT, Op::INSERT));
      if (original_ == target_)
        next_cost_ =
            std::min(next_cost_, *previous_insert_cost_ +
                                     CostOfOperation(Op::INSERT, Op::NEXT));
    }
    if (previous_next_cost_) {
      delete_cost_ =
          std::min(delete_cost_, *previous_next_cost_ +
                                     CostOfOperation(Op::NEXT, Op::DELETE));
      insert_cost_ =
          std::min(insert_cost_, *previous_next_cost_ +
                                     CostOfOperation(Op::NEXT, Op::INSERT));
      if (original_ == target_)
        next_cost_ =
            std::min(next_cost_, *previous_next_cost_ +
                                     CostOfOperation(Op::NEXT, Op::NEXT));
    }
  }
};
}  // namespace

void InertialGraph::Compute(const Byte *original, std::size_t original_length,
                            const Byte *target, std::size_t target_length) {
  /*
   * Construct a rectangular graph as the edit graph.
   * The edit graph supports down (insert), right (delete), and
   * diagonal down-right (next) traversal.  The edit graph
   * is a digraph without cycles and can be traversed in
   * O(n^2) complexity and memory.
   */

  std::size_t xlen = original_length + 1;
  std::size_t ylen = target_length + 1;

  std::unique_ptr<Byte[]> xval(new Byte[xlen]);
  std::unique_ptr<Byte[]> yval(new Byte[ylen]);

  xval[0] = 0;
  yval[0] = 0;
  std::copy(original, original + original_length, &xval[1]);
  std::copy(target, target + target_length, &yval[1]);

  std::vector<std::vector<Node>> nodes(ylen, std::vector<Node>(xlen));
  /*
   * Actually traverse the graph, using dynamic programming to store
   * the best path.
   */

  for (std::size_t y = 0; y < ylen; ++y) {
    for (std::size_t x = 0; x < xlen; ++x) {
      Node &node = nodes[y][x];
      //			printf("Node at x=%iy=%i",x,y);
      if (x == 0 && y == 0) {
        node.delete_cost_ = 0;
        node.insert_cost_ = 0;
        node.next_cost_ = 0;
      } else if (x == 0) {
        node.previous_insert_cost_ = &nodes[y - 1][x].insert_cost_;
      } else if (y == 0) {
        node.previous_delete_cost_ = &nodes[y][x - 1].delete_cost_;
      } else { /* x > 0 && y > 0 */
        node.previous_delete_cost_ = &nodes[y - 1][x].delete_cost_;
        node.previous_insert_cost_ = &nodes[y][x - 1].insert_cost_;
        node.previous_next_cost_ = &nodes[y - 1][x - 1].next_cost_;
      }
      node.original_ = xval[x];
      node.target_ = yval[y];
      node.Compute();
      //			printf( " costs delete=%i insert=%i next=%i\n",
      // node.delete_cost_, node.insert_cost_, node.next_cost_);
    }
  }

  std::vector<Op> ret;
  std::vector<Byte> buf;
  Op::Type op = Op::STOP;
  std::size_t run = 0;

  /*
   * Traverse backward from lower right corner to upper left corner.
   * This gives the edit script in reverse order.
   */

  op_queue_.clear();
  op_queue_.push_back(Op(Op::STOP, 1));
  std::size_t x = xlen - 1, y = ylen - 1;
  while (x > 0 || y > 0) {
    Node &node = nodes[y][x];
    if (x > 0 && y > 0 && node.next_cost_ <= node.delete_cost_ &&
        node.next_cost_ <= node.insert_cost_) {
      if (op_queue_.back().GetType() != Op::NEXT) {
        op_queue_.push_back(Op(Op::NEXT, 1));
      } else {
        op_queue_.back().MutableLength()++;
      }
      x -= 1;
      y -= 1;
    } else if (x > 0 && node.delete_cost_ <= node.insert_cost_ &&
               node.delete_cost_ <= node.next_cost_) {
      if (op_queue_.back().GetType() != Op::DELETE) {
        op_queue_.push_back(Op(Op::DELETE, 1));
      } else {
        op_queue_.back().MutableLength()++;
      }
      x -= 1;
    } else if (y > 0 && node.insert_cost_ <= node.delete_cost_ &&
               node.next_cost_ <= node.next_cost_) {
      if (op_queue_.back().GetType() != Op::INSERT) {
        ByteArray data(new Byte[1]);
        data[0] = node.target_;
        op_queue_.push_back(Op(Op::INSERT, 1, std::move(data)));
      } else {
        ByteArray data(new Byte[op_queue_.back().GetLength() + 1]);
        std::copy(
            op_queue_.back().GetValue().get(),  //
            op_queue_.back().GetValue().get() + op_queue_.back().GetLength(),
            data.get());
        data[op_queue_.back().GetLength()] = node.target_;
        op_queue_.back().MutableValue() = std::move(data);
        op_queue_.back().MutableLength()++;
      }
      y -= 1;
    }
  }
  op_queue_.erase(op_queue_.begin());

  auto op_queue = std::move(op_queue_);
  op_queue_.resize(op_queue.size());
  std::move(op_queue.rbegin(), op_queue.rend(), op_queue_.begin());
}

std::unique_ptr<q::OpQueue> InertialGraph::MakeOpQueue() const {
  /*
   * Copy the edit script and return it as an OpQueue.
   */
  std::vector<Op> queue;
  for (const auto &op : op_queue_) {
    ByteArray value;
    if (op.GetValue()) {
      value.reset(new Byte[op.GetLength()]);
      std::copy(op.GetValue().get(), op.GetValue().get() + op.GetLength(),
                value.get());
    }
    queue.push_back(Op(op.GetType(), op.GetLength(),
                       std::move(value)));  // @suppress("Ambiguous problem")
  }
  return std::unique_ptr<q::OpQueue>(new q::VectorOpQueue(std::move(queue)));
}

}  // namespace alg
}  // namespace badiff
