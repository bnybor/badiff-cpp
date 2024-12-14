#include <badiff/badiff.hpp>
#include <badiff/op.hpp>

#include <badiff/alg/edit_graph.hpp>
#include <badiff/alg/graph.hpp>
#include <badiff/alg/inertial_graph.hpp>

#include <badiff/q/chunking_op_queue.hpp>
#include <badiff/q/coalescing_op_queue.hpp>
#include <badiff/q/graph_op_queue.hpp>
#include <badiff/q/minimize_op_queue.hpp>
#include <badiff/q/op_queue.hpp>
#include <badiff/q/replace_op_queue.hpp>
#include <badiff/q/stream_replace_op_queue.hpp>

#include <sstream>

namespace badiff {
namespace {
std::unique_ptr<q::OpQueue> Wrap(std::unique_ptr<q::OpQueue> op_queue) {

  op_queue.reset(new q::ChunkingOpQueue(std::move(op_queue), 8192));

  op_queue.reset(
      new q::GraphOpQueue(std::move(op_queue),
                          std::unique_ptr<alg::Graph>(new alg::InertialGraph)));

  //  op_queue.reset(new q::ChunkingOpQueue(std::move(op_queue), 8192));
  //
  //  op_queue.reset(
  //      new q::GraphOpQueue(std::move(op_queue),
  //                          std::unique_ptr<alg::Graph>(new
  //                          alg::InertialGraph)));

  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  op_queue.reset(new q::MinimizeOpQueue(std::move(op_queue)));

  return op_queue;
}
} // namespace

std::unique_ptr<Diff> Diff::Make(const char *original, int original_size,
                                 const char *target, int target_size) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::ReplaceOpQueue(original, original_size, target, target_size));

  op_queue = Wrap(std::move(op_queue));

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Diff> diff(new Diff);
  diff->len = str.size();
  diff->diff.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->diff.get());

  return diff;
}

std::unique_ptr<Diff> Diff::Make(std::istream &original, std::istream &target) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::StreamReplaceOpQueue(original, target, 8192));

  op_queue = Wrap(std::move(op_queue));

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Diff> diff(new Diff);
  diff->len = str.size();
  diff->diff.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->diff.get());

  return diff;
}

void Diff::Apply(std::istream &original, std::ostream &target) {
  std::istringstream in(std::string(diff.get(), len));
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);

  op_queue->Deserialize(in);
  op_queue->Apply(original, target);
}

} // namespace badiff
