#include <badiff/alg/inertial_graph.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class InertialGraphTest : public testing::Test {
public:
  virtual ~InertialGraphTest() = default;
};

TEST_F(InertialGraphTest, TestIdentity) {
  alg::InertialGraph graph;
  std::string hello = "hello";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), hello.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string(">5"));
}

TEST_F(InertialGraphTest, TestHelloWorld) {
  alg::InertialGraph graph;
  std::string hello = "hello";
  std::string world = "world";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), world.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("-5+5"));
}

TEST_F(InertialGraphTest, TestHelloCruelWorld) {
  alg::InertialGraph graph;
  std::string hello = "hello world";
  std::string world = "hellish cruel world";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), world.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string(">4-1+9>6"));
}

TEST_F(InertialGraphTest, TestHelloAB) {
  alg::InertialGraph graph;
  std::string hello = "ab";
  std::string world = "bb";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), world.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("-1+1>1"));
}
TEST_F(InertialGraphTest, TestHelloA8B8) {
  alg::InertialGraph graph;
  std::string hello = "aaaaaaaabbbbbbbb";
  std::string world = "bbbbbbbbbbbbbbbb";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), world.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("-8+8>8"));
}
