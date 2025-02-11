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

#include <badiff/q/rewinding_op_queue.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class RewindingOpQueueTest : public testing::Test {
public:
  virtual ~RewindingOpQueueTest() = default;
};

TEST_F(RewindingOpQueueTest, RewindDeleteTest) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op(Op::INSERT, "a"));
  op_queue->Push(Op(Op::INSERT, "b"));
  op_queue->Push(Op(Op::DELETE, "a"));

  op_queue.reset(new q::RewindingOpQueue(std::move(op_queue)));
  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), ">1+1");
}

TEST_F(RewindingOpQueueTest, RewindInsertTest) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op(Op::DELETE, "a"));
  op_queue->Push(Op(Op::DELETE, "b"));
  op_queue->Push(Op(Op::INSERT, "a"));

  op_queue.reset(new q::RewindingOpQueue(std::move(op_queue)));
  ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), ">1-1");
}
