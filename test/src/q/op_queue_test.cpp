#include <badiff/alg/inertial_graph.hpp>
#include <badiff/q/graph_op_queue.hpp>
#include <badiff/q/op_queue.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class OpQueueTest : public testing::Test {
public:
  virtual ~OpQueueTest() = default;
};

TEST_F(OpQueueTest, SerDe) {
  auto graph = std::unique_ptr<alg::Graph>(new alg::InertialGraph);

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::OpQueue);
  op_queue->Push(Op(Op::DELETE, "a"));
  op_queue->Push(Op(Op::INSERT, "b"));
  op_queue->Push(Op(Op::DELETE, "b"));

  std::ostringstream os;
  op_queue->Serialize(os);
  std::istringstream is(os.str());
  op_queue.reset(new q::OpQueue);
  op_queue->Deserialize(is);

  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "a"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "b"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "b"));
  ASSERT_TRUE(op_queue->IsEmpty());
}
