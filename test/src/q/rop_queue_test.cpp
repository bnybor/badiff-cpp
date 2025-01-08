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
