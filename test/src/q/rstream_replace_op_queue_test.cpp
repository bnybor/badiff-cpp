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

  auto op0 = *op_queue->Pop();
  auto op1 = *op_queue->Pop();
  ASSERT_TRUE(op_queue->IsEmpty());
}
