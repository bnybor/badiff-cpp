#ifndef BADIFF_Q_REPLACE_OP_QUEUE_HPP_
#define BADIFF_Q_REPLACE_OP_QUEUE_HPP_

#include <badiff/bytes.hpp>
#include <badiff/q/op_queue.hpp>

namespace badiff {
namespace q {

class ReplaceOpQueue : public OpQueue {
public:
  ReplaceOpQueue(Byte *original, std::size_t original_size, Byte *target,
                 std::size_t target_size);
};

} // namespace q
} // namespace badiff

#endif /* BADIFF_Q_REPLACE_OP_QUEUE_HPP_ */
