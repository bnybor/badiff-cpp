/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <badiff/alg/edit_graph.hpp>
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

  std::istringstream in(std::string(original.get(), hello.size()));
  std::ostringstream out;
  op_queue->Apply(in, out);

  ASSERT_EQ(out.str(), hello);
}

TEST_F(GraphOpQueueTest, TestHelloWorld) {
  std::string hello = "hello";
  std::string world = "world";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  {
    auto graph = std::unique_ptr<alg::Graph>(new alg::InertialGraph);
    auto op_queue = std::unique_ptr<q::OpQueue>(new q::ReplaceOpQueue(
        original.get(), hello.size(), target.get(), world.size()));

    op_queue.reset(new q::GraphOpQueue(std::move(op_queue), std::move(graph)));

    std::istringstream in(std::string(original.get(), hello.size()));
    std::ostringstream out;
    op_queue->Apply(in, out);

    ASSERT_EQ(out.str(), world);
  }

  {
    auto graph = std::unique_ptr<alg::Graph>(new alg::EditGraph);
    auto op_queue = std::unique_ptr<q::OpQueue>(new q::ReplaceOpQueue(
        original.get(), hello.size(), target.get(), world.size()));

    op_queue.reset(new q::GraphOpQueue(std::move(op_queue), std::move(graph)));

    std::istringstream in(std::string(original.get(), hello.size()));
    std::ostringstream out;
    op_queue->Apply(in, out);

    ASSERT_EQ(out.str(), world);
  }
}

TEST_F(GraphOpQueueTest, TestHelloAB) {
  auto graph = std::unique_ptr<alg::Graph>(new alg::InertialGraph);
  std::string hello = "ab";
  std::string world = "bb";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::ReplaceOpQueue(
      original.get(), hello.size(), target.get(), world.size()));

  op_queue.reset(new q::GraphOpQueue(std::move(op_queue), std::move(graph)));

  std::istringstream in(std::string(original.get(), hello.size()));
  std::ostringstream out;
  op_queue->Apply(in, out);

  ASSERT_EQ(out.str(), world);
}

TEST_F(GraphOpQueueTest, TestHelloA8B8) {
  auto graph = std::unique_ptr<alg::Graph>(new alg::InertialGraph);
  std::string hello = "aaaaaaaabbbbbbbb";
  std::string world = "bbbbbbbbbbbbbbbb";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::ReplaceOpQueue(
      original.get(), hello.size(), target.get(), world.size()));

  op_queue.reset(new q::GraphOpQueue(std::move(op_queue), std::move(graph)));

  std::istringstream in(std::string(original.get(), hello.size()));
  std::ostringstream out;
  op_queue->Apply(in, out);

  ASSERT_EQ(out.str(), world);
}

TEST_F(GraphOpQueueTest, TestHelloABB) {
  auto graph = std::unique_ptr<alg::Graph>(new alg::InertialGraph);

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::OpQueue);
  op_queue->Push(Op(Op::DELETE, "a"));
  op_queue->Push(Op(Op::INSERT, "b"));
  op_queue->Push(Op(Op::DELETE, "b"));

  op_queue.reset(new q::GraphOpQueue(std::move(op_queue), std::move(graph)));

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), "-1>1");
}
