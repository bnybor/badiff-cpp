#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

FilterOpQueue::FilterOpQueue(std::unique_ptr<OpQueue> source)
    : source_(std::move(source)) {}

void FilterOpQueue::Push(Op op) { source_->Push(std::move(op)); }

bool FilterOpQueue::Require(std::size_t count) {
  while (filtering_.size() < count) {
    auto maybe_op = source_->Pop();
    if (!maybe_op)
      return false;
    filtering_.push_back(std::move(*maybe_op));
  }
  return true;
}

void FilterOpQueue::Drop(std::size_t count) {
  filtering_.erase(filtering_.begin(), filtering_.begin() + count);
}

bool FilterOpQueue::Flush() {
  bool flushed = false;
  for (auto &e : filtering_) {
    flushed = true;
    Prepare(std::move(e));
  }
  filtering_.clear();
  return flushed;
}

} // namespace q
} // namespace badiff