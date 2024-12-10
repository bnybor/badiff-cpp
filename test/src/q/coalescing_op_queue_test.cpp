#include <badiff/q/coalescing_op_queue.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class CoalescingOpQueueTest : public testing::Test {
public:
  virtual ~CoalescingOpQueueTest() = default;
};

TEST_F(CoalescingOpQueueTest, DD) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op(Op::DELETE, 1));
  op_queue->Push(Op(Op::DELETE, 1));
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  ASSERT_FALSE(op_queue->IsEmpty());
  Op op = *op_queue->Pop();
  ASSERT_EQ(op.GetType(), Op::DELETE);
  ASSERT_EQ(op.GetLength(), 2);
  ASSERT_TRUE(op_queue->IsEmpty());
}
