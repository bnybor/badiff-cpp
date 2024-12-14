#include <badiff/q/vector_op_queue.hpp>

namespace badiff {
namespace q {

VectorOpQueue::VectorOpQueue(std::vector<Op> vec) {
  for (auto &op : vec) {
    Push(std::move(op));
  }
}

VectorOpQueue::~VectorOpQueue() {}

} // namespace q
} // namespace badiff
