#include <badiff/q/rstream_replace_op_queue.hpp>

#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class RStreamReplaceOpQueueTest : public testing::Test {
public:
  virtual ~RStreamReplaceOpQueueTest() = default;
};

TEST_F(RStreamReplaceOpQueueTest, Reversed) {
  std::string hello("hello");
  std::string world("world");

  std::istringstream original(hello);
  std::istringstream target(world);

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::RStreamReplaceOpQueue(
      original, hello.size(), target, world.size(), 999));

  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "olleh"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "dlrow"));
  ASSERT_TRUE(op_queue->IsEmpty());
}

TEST_F(RStreamReplaceOpQueueTest, TinyChunk) {
  std::string hello("hello");
  std::string world("world");

  std::istringstream original(hello);
  std::istringstream target(world);

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::RStreamReplaceOpQueue(
      original, hello.size(), target, world.size(), 1));

  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "o"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "d"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "l"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "l"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "l"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "r"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "e"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "o"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "h"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "w"));
  ASSERT_TRUE(op_queue->IsEmpty());
}
