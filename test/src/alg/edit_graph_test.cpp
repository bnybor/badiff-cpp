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

#include <stdio.h>
#include <stdlib.h>

#include <badiff/alg/edit_graph.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class EditGraphTest : public testing::Test {
public:
  virtual ~EditGraphTest() = default;
};

TEST_F(EditGraphTest, TestIdentity) {
  alg::EditGraph graph;
  std::string hello = "hello";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), hello.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string(">5"));
}

TEST_F(EditGraphTest, TestHelloWorld) {
  alg::EditGraph graph;
  std::string hello = "hello";
  std::string world = "world";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), world.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue),
            std::string("+3-2>1+1-2"));
}

TEST_F(EditGraphTest, TestHelloHellish) {
  alg::EditGraph graph;
  std::string hello = "hello world";
  std::string world = "hellish cruel world";

  std::unique_ptr<char[]> original(new char[hello.size()]);
  std::copy(hello.begin(), hello.end(), original.get());

  std::unique_ptr<char[]> target(new char[world.size()]);
  std::copy(world.begin(), world.end(), target.get());

  graph.Compute(original.get(), hello.size(), target.get(), world.size());

  auto op_queue = graph.MakeOpQueue();

  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue),
            std::string(">2+1>1+8>1-1>6"));
}
