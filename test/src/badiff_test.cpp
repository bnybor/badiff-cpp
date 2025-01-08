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
