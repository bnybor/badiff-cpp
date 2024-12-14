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
