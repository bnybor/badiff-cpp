#include <badiff/badiff.hpp>
#include <badiff/op.hpp>

#include <badiff/alg/graph.hpp>
#include <badiff/alg/inertial_graph.hpp>

#include <badiff/q/chunking_op_queue.hpp>
#include <badiff/q/coalescing_op_queue.hpp>
#include <badiff/q/graph_op_queue.hpp>
#include <badiff/q/op_queue.hpp>
#include <badiff/q/replace_op_queue.hpp>

namespace badiff {
std::unique_ptr<Diff> Diff::MakeDiff(const char *original, int original_size,
                                     const char *target, int target_size) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::ReplaceOpQueue(original, original_size, target, target_size));

  op_queue.reset(new q::ChunkingOpQueue(std::move(op_queue), 8192));

  op_queue.reset(
      new q::GraphOpQueue(std::move(op_queue),
                          std::unique_ptr<alg::Graph>(new alg::InertialGraph)));

  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));

  op_queue.reset(new q::ChunkingOpQueue(std::move(op_queue), 8192));

  op_queue.reset(
      new q::GraphOpQueue(std::move(op_queue),
                          std::unique_ptr<alg::Graph>(new alg::InertialGraph)));

  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Diff> diff(new Diff);
  diff->len = str.size();
  diff->op_queue.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->op_queue.get());

  return diff;
}

} // namespace badiff
