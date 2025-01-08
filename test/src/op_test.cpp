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

#include <badiff/op.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

using namespace badiff;

class OpTest : public testing::Test {
public:
  virtual ~OpTest() = default;
};

TEST_F(OpTest, SerDeSTOP) {
  Op op(Op::STOP, 554, nullptr);
  std::ostringstream os;
  op.Serialize(os);
  std::istringstream is(os.str());
  Op op2;
  op2.Deserialize(is);
  ASSERT_EQ(op2.GetType(), Op::STOP);
  ASSERT_EQ(op2.GetLength(), 554);
  ASSERT_EQ(op2.GetValue(), nullptr);
}
TEST_F(OpTest, SerDeINSERT) {
  std::unique_ptr<char[]> abc(new char[3]);
  abc[0] = 'a';
  abc[1] = 'b';
  abc[2] = 'c';
  Op op(Op::INSERT, 3, std::move(abc));
  std::ostringstream os;
  op.Serialize(os);
  std::istringstream is(os.str());
  Op op2;
  op2.Deserialize(is);
  ASSERT_EQ(op2.GetType(), Op::INSERT);
  ASSERT_EQ(op2.GetLength(), 3);
  ASSERT_EQ(op2.GetValue()[0], 'a');
  ASSERT_EQ(op2.GetValue()[1], 'b');
  ASSERT_EQ(op2.GetValue()[2], 'c');
}
