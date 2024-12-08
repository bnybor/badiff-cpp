#include <badiff/alg/inertial_graph.hpp>
#include <badiff/q/graph_op_queue.hpp>
#include <badiff/q/replace_op_queue.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class GraphOpQueueTest : public testing::Test {
public:
  virtual ~GraphOpQueueTest() = default;
};

TEST_F(GraphOpQueueTest, TestIdentity) {
  auto graph = std::unique_ptr<alg::Graph>(new alg::InertialGraph);
  std::string hello = "hello";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), target.get());

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::ReplaceOpQueue(
      original.get(), hello.size(), target.get(), hello.size()));

  op_queue.reset(new q::GraphOpQueue(std::move(op_queue), std::move(graph)));

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string(">5"));
}
//
// TEST_F(GraphOpQueueTest, TestHelloWorld) {
//  alg::InertialGraph graph;
//  std::string hello = "hello";
//  std::string world = "world";
//
//  std::unique_ptr<char[]> original(new char[hello.size()]);
//  std::copy(hello.begin(), hello.end(), original.get());
//
//  std::unique_ptr<char[]> target(new char[world.size()]);
//  std::copy(world.begin(), world.end(), target.get());
//
//  graph.Compute(original.get(), hello.size(), target.get(), world.size());
//
//  auto op_queue = graph.MakeOpQueue();
//
//  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("+5-5"));
//}
//
// TEST_F(GraphOpQueueTest, TestHelloAB) {
//  alg::InertialGraph graph;
//  std::string hello = "ab";
//  std::string world = "bb";
//
//  std::unique_ptr<char[]> original(new char[hello.size()]);
//  std::copy(hello.begin(), hello.end(), original.get());
//
//  std::unique_ptr<char[]> target(new char[world.size()]);
//  std::copy(world.begin(), world.end(), target.get());
//
//  graph.Compute(original.get(), hello.size(), target.get(), world.size());
//
//  auto op_queue = graph.MakeOpQueue();
//
//  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("+2-2"));
//}
// TEST_F(GraphOpQueueTest, TestHelloA8B8) {
//  alg::InertialGraph graph;
//  std::string hello = "aaaaaaaabbbbbbbb";
//  std::string world = "bbbbbbbbbbbbbbbb";
//
//  std::unique_ptr<char[]> original(new char[hello.size()]);
//  std::copy(hello.begin(), hello.end(), original.get());
//
//  std::unique_ptr<char[]> target(new char[world.size()]);
//  std::copy(world.begin(), world.end(), target.get());
//
//  graph.Compute(original.get(), hello.size(), target.get(), world.size());
//
//  auto op_queue = graph.MakeOpQueue();
//
//  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue),
//  std::string("+10-10>6"));
//}
