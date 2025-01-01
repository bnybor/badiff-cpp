#include <badiff/q/minimize_op_queue.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class MinimizeOpQueueTest : public testing::Test {
public:
  virtual ~MinimizeOpQueueTest() = default;
};

TEST_F(MinimizeOpQueueTest, Minimize) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op(Op::INSERT, "foo"));
  op_queue->Push(Op(Op::DELETE, "bar"));
  op_queue->Push(Op(Op::NEXT, "qux"));

  op_queue.reset(new q::MinimizeOpQueue(std::move(op_queue)));

  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "foo"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, 3));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::NEXT, 3));
}
