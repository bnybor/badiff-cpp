#include <badiff/q/rop_queue.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class ROpQueueTest : public testing::Test {
public:
  virtual ~ROpQueueTest() = default;
};

TEST_F(ROpQueueTest, Reverse) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op(Op::DELETE, "foo"));
  op_queue->Push(Op(Op::INSERT, "bar"));
  op_queue->Push(Op(Op::NEXT, 3));

  op_queue.reset(new q::ROpQueue(std::move(op_queue)));

  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "oof"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "rab"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::NEXT, 3));
  ASSERT_TRUE(op_queue->IsEmpty());
}
