#include <badiff/q/replace_op_queue.hpp>

#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class ReplaceOpQueueTest : public testing::Test {
public:
  virtual ~ReplaceOpQueueTest() = default;
};

TEST_F(ReplaceOpQueueTest, TinyChunk) {
  std::string hello("hello");
  std::string world("world");

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::ReplaceOpQueue(
      hello.c_str(), hello.size(), world.c_str(), world.size(), 1));

  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "h"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "w"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "e"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "o"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "l"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "r"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "l"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "l"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "o"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "d"));
  ASSERT_TRUE(op_queue->IsEmpty());
}
