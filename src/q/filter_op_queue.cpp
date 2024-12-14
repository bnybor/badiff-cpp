#include <badiff/q/filter_op_queue.hpp>

namespace badiff {
namespace q {

FilterOpQueue::FilterOpQueue(std::unique_ptr<OpQueue> source)
    : source_(std::move(source)) {}

FilterOpQueue::~FilterOpQueue() {}

void FilterOpQueue::Push(Op op) { source_->Push(std::move(op)); }

bool FilterOpQueue::Require(std::size_t count) {
  while (filtering_.size() < count) {
    if (source_->IsEmpty())
      return false;
    filtering_.push_back(*source_->Pop());
  }
  return true;
}

void FilterOpQueue::Drop(std::size_t count) {
  filtering_.erase(filtering_.begin(), filtering_.begin() + count);
}

bool FilterOpQueue::Flush(std::size_t n) {
  while (!filtering_.empty()) {
    auto &e = filtering_.front();
    if (n-- == 0)
      return true;
    Prepare(std::move(e));
    filtering_.erase(filtering_.begin());
  }
  return true;
}

} // namespace q
} // namespace badiff
