#include <badiff/badiff.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

using namespace badiff;

class BadiffTest : public testing::Test {
public:
  virtual ~BadiffTest() = default;
};

TEST_F(BadiffTest, DiffHelloWorld) {
  std::string hello("hello worlddddddddddddddddddddddddddd");
  std::string world("hellish cruel worlddddddddddddddddddddddddddd");

  auto diff =
      Delta::Make(hello.c_str(), hello.size(), world.c_str(), world.size());

  std::istringstream original(hello);
  std::ostringstream target;

  diff->Apply(original, target);
  ASSERT_EQ(target.str(), world);

  original.clear();
  std::unique_ptr<char[]> target_buf(new char[diff->target_len_]);

  diff->Apply(hello.c_str(), target_buf.get());
  ASSERT_EQ(std::string(target_buf.get(), diff->target_len_), world);
}

TEST_F(BadiffTest, aba) {
  std::string hello("abcdefghi");
  std::string world("xyzdefxyz");

  auto diff =
      Delta::Make(hello.c_str(), hello.size(), world.c_str(), world.size());

  std::istringstream original(hello);
  std::ostringstream target;

  diff->Apply(original, target);
  ASSERT_EQ(target.str(), world);

  original.clear();
  std::unique_ptr<char[]> target_buf(new char[diff->target_len_]);

  diff->Apply(hello.c_str(), target_buf.get());
  ASSERT_EQ(std::string(target_buf.get(), diff->target_len_), world);
}
