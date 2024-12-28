#include <badiff/badiff.hpp>
#include <badiff/op.hpp>

#include <badiff/defaults.hpp>

#include <badiff/alg/edit_graph.hpp>
#include <badiff/alg/graph.hpp>
#include <badiff/alg/inertial_graph.hpp>

#include <badiff/q/chunking_op_queue.hpp>
#include <badiff/q/coalescing_op_queue.hpp>
#include <badiff/q/compacting_op_queue.hpp>
#include <badiff/q/graph_op_queue.hpp>
#include <badiff/q/minimize_op_queue.hpp>
#include <badiff/q/op_queue.hpp>
#include <badiff/q/replace_op_queue.hpp>
#include <badiff/q/stream_replace_op_queue.hpp>

#include <sstream>

namespace badiff {
bool CONSOLE_OUTPUT = false;

namespace {
std::unique_ptr<q::OpQueue> Wrap(std::unique_ptr<q::OpQueue> op_queue) {

  op_queue.reset(
      new q::GraphOpQueue(std::move(op_queue),
                          std::unique_ptr<alg::Graph>(new alg::InertialGraph)));

  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  op_queue.reset(new q::CompactingOpQueue(std::move(op_queue)));

  op_queue.reset(new q::ChunkingOpQueue(std::move(op_queue)));

  op_queue.reset(
      new q::GraphOpQueue(std::move(op_queue),
                          std::unique_ptr<alg::Graph>(new alg::InertialGraph)));

  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  op_queue.reset(new q::CompactingOpQueue(std::move(op_queue)));

  op_queue.reset(new q::MinimizeOpQueue(std::move(op_queue)));

  return op_queue;
}
} // namespace

std::unique_ptr<Diff> Diff::Make(const char *original, int original_len,
                                 const char *target, int target_len) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::ReplaceOpQueue(original, original_len, target, target_len));

  op_queue.reset(new q::ChunkingOpQueue(std::move(op_queue)));

  op_queue = Wrap(std::move(op_queue));

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Diff> diff(new Diff);
  diff->len_ = str.size();
  diff->diff_.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->diff_.get());

  return diff;
}

std::unique_ptr<Diff> Diff::Make(std::istream &original, int original_len,
                                 std::istream &target, int target_len) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::StreamReplaceOpQueue(original, original_len, target, target_len));

  op_queue = Wrap(std::move(op_queue));

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Diff> diff(new Diff);
  diff->len_ = str.size();
  diff->diff_.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->diff_.get());

  return diff;
}

std::unique_ptr<Diff> Diff::Make(std::istream &original, std::istream &target) {
  std::unique_ptr<q::OpQueue> op_queue(
      new q::StreamReplaceOpQueue(original, target));

  op_queue = Wrap(std::move(op_queue));

  std::ostringstream ss;
  op_queue->Serialize(ss);
  auto str = ss.str();

  std::unique_ptr<Diff> diff(new Diff);
  diff->len_ = str.size();
  diff->diff_.reset(new char[str.size()]);

  std::copy(str.c_str(), str.c_str() + str.size(), diff->diff_.get());

  return diff;
}

void Diff::Apply(std::istream &original, std::ostream &target) {
  std::istringstream in(std::string(diff_.get(), len_));
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);

  op_queue->Deserialize(in);
  op_queue->Apply(original, target);
}

} // namespace badiff
