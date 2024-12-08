#include <badiff/q/replace_op_queue.hpp>

namespace badiff {
namespace q {

ReplaceOpQueue::ReplaceOpQueue(Byte *original, std::size_t original_size,
                               Byte *target, std::size_t target_size) {
  if (original_size) {
    ByteArray o(new Byte[original_size]);
    std::copy(original, original + original_size, o.get());
    Prepare(Op(Op::DELETE, original_size, std::move(o)));
  }
  if (target_size) {
    ByteArray t(new Byte[target_size]);
    std::copy(target, target + target_size, t.get());
    Prepare(Op(Op::INSERT, original_size, std::move(t)));
  }
}

} // namespace q
} // namespace badiff
