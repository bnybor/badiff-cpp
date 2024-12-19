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

  auto diff = Diff::Make(hello.c_str(), hello.size(), world.c_str(),
                         world.size(), Diff::NORMAL_CHUNK);

  std::istringstream original(hello);
  std::ostringstream target;

  diff->Apply(original, target);
  ASSERT_EQ(target.str(), world);
}
