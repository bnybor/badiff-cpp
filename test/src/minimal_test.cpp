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

#include <badiff/minimal.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>

using namespace badiff;

class MinimalTest : public testing::Test {
public:
  virtual ~MinimalTest() = default;
};

TEST_F(MinimalTest, DiffHelloWorld) {
  std::string hello("hello worlddddddddddddddddddddddddddd");
  std::string world("hellish cruel worlddddddddddddddddddddddddddd");

  char diff[1024];
  int diff_len;

  diff_len = badiff::diff(diff, sizeof(diff), hello.c_str(), hello.size(), world.c_str(), world.size());

  ASSERT_NE(diff_len, -1);

  char world2[1024];
  int world2_len;

  world2_len = badiff::patch(world2, sizeof(world2), hello.c_str(), hello.size(), diff, diff_len);

  ASSERT_NE(world2_len, -1);
  
  ASSERT_EQ(std::string(world2, world2 + world2_len), world);
}

TEST_F(MinimalTest, aba) {
  std::string hello("abcdefghi");
  std::string world("xyzdefxyz");

  char diff[1024];
  int diff_len;

  diff_len = badiff::diff(diff, sizeof(diff), hello.c_str(), hello.size(), world.c_str(), world.size());

  ASSERT_NE(diff_len, -1);

  char world2[1024];
  int world2_len;

  world2_len = badiff::patch(world2, sizeof(world2), hello.c_str(), hello.size(), diff, diff_len);

  ASSERT_NE(world2_len, -1);
  
  ASSERT_EQ(std::string(world2, world2 + world2_len), world);
}
