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

#include <badiff/q/rreplace_op_queue.hpp>

#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class RReplaceOpQueueTest : public testing::Test {
public:
  virtual ~RReplaceOpQueueTest() = default;
};

TEST_F(RReplaceOpQueueTest, Reversed) {
  std::string hello("hello");
  std::string world("world");

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::RReplaceOpQueue(
      hello.c_str(), hello.size(), world.c_str(), world.size(), 999));

  ASSERT_EQ(*op_queue->Pop(), Op(Op::DELETE, "olleh"));
  ASSERT_EQ(*op_queue->Pop(), Op(Op::INSERT, "dlrow"));
  ASSERT_TRUE(op_queue->IsEmpty());
}

TEST_F(RReplaceOpQueueTest, TinyChunk) {
  std::string hello("hello");
  std::string world("world");

  auto op_queue = std::unique_ptr<q::OpQueue>(new q::RReplaceOpQueue(
      hello.c_str(), hello.size(), world.c_str(), world.size(), 1));

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
