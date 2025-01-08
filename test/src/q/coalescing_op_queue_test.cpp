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

#include <badiff/q/coalescing_op_queue.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace badiff;

class CoalescingOpQueueTest : public testing::Test {
public:
  virtual ~CoalescingOpQueueTest() = default;
};

TEST_F(CoalescingOpQueueTest, DD) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op::DELETE);
  op_queue->Push(Op::DELETE);
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  ASSERT_FALSE(op_queue->IsEmpty());
  Op op = *op_queue->Pop();
  ASSERT_EQ(op.GetType(), Op::DELETE);
  ASSERT_EQ(op.GetLength(), 2);
  ASSERT_TRUE(op_queue->IsEmpty());
}

TEST_F(CoalescingOpQueueTest, DDD) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op::DELETE);
  op_queue->Push(Op::DELETE);
  op_queue->Push(Op::DELETE);
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  ASSERT_FALSE(op_queue->IsEmpty());
  Op op = *op_queue->Pop();
  ASSERT_EQ(op.GetType(), Op::DELETE);
  ASSERT_EQ(op.GetLength(), 3);
  ASSERT_TRUE(op_queue->IsEmpty());
}

TEST_F(CoalescingOpQueueTest, DID) {
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op::DELETE);
  op_queue->Push(Op::INSERT);
  op_queue->Push(Op::DELETE);
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  ASSERT_FALSE(op_queue->IsEmpty());
  Op op;
  op = std::move(*op_queue->Pop());
  ASSERT_EQ(op.GetType(), Op::DELETE);
  ASSERT_EQ(op.GetLength(), 2);
  op = std::move(*op_queue->Pop());
  ASSERT_EQ(op.GetType(), Op::INSERT);
  ASSERT_EQ(op.GetLength(), 1);
  ASSERT_TRUE(op_queue->IsEmpty());
}

TEST_F(CoalescingOpQueueTest, DIID) {
  /*
   * Order of coalescing
   * D1 I1 I1 D1
   * I2 D1 D1
   * D2 I2
   */
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op::DELETE);
  op_queue->Push(Op::INSERT);
  op_queue->Push(Op::INSERT);
  op_queue->Push(Op::DELETE);
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  ASSERT_FALSE(op_queue->IsEmpty());
  Op op;
  op = std::move(*op_queue->Pop());
  ASSERT_EQ(op.GetType(), Op::DELETE);
  ASSERT_EQ(op.GetLength(), 2);
  op = std::move(*op_queue->Pop());
  ASSERT_EQ(op.GetType(), Op::INSERT);
  ASSERT_EQ(op.GetLength(), 2);
  ASSERT_TRUE(op_queue->IsEmpty());
}

TEST_F(CoalescingOpQueueTest, IDDI) {
  /*
   * Order of coalescing
   * I1 D1 D1 I1
   * D2 I1 I1
   * I2 D2
   * D2 I2
   */
  std::unique_ptr<q::OpQueue> op_queue(new q::OpQueue);
  op_queue->Push(Op::INSERT);
  op_queue->Push(Op::DELETE);
  op_queue->Push(Op::DELETE);
  op_queue->Push(Op::INSERT);
  op_queue.reset(new q::CoalescingOpQueue(std::move(op_queue)));
  ASSERT_FALSE(op_queue->IsEmpty());
  Op op;
  op = std::move(*op_queue->Pop());
  ASSERT_EQ(op.GetType(), Op::DELETE);
  ASSERT_EQ(op.GetLength(), 2);
  op = std::move(*op_queue->Pop());
  ASSERT_EQ(op.GetType(), Op::INSERT);
  ASSERT_EQ(op.GetLength(), 2);
  ASSERT_TRUE(op_queue->IsEmpty());
}
