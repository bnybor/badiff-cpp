#include <badiff/badiff.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>

class FnPatchTest : public testing::Test {
public:
  virtual ~FnPatchTest() = default;
};

TEST_F(FnPatchTest, PatchStruct) {
  /*
   * Create a patch from a struct, then apply the patch to a copy of the
   * original.  This demonstrates patching runtime data and not just files.
   */
  struct Data {
    int a;
    int b;
    int c;
  };

  auto d1 = Data{.a = 1, .b = 2, .c = 3};
  auto d2 = Data{.a = 1, .b = 4, .c = 3};
  // The difference between d1 and d2 is that b was changed to 4.
  auto delta = badiff::Delta::Make( //
      (char *)&d1, sizeof(d1),      //
      (char *)&d2, sizeof(d2));

  auto d3 = Data{.a = 1, .b = 2, .c = 3};
  auto d4 = Data{.a = 0, .b = 0, .c = 0};
  // Apply that difference to d3, storing in d4.
  delta->Apply((char *)&d3, (char *)&d4);

  // d4 should only differ from d3 in that b was changed to 4.
  ASSERT_EQ(d4.a, d2.a);
  ASSERT_EQ(d4.b, d2.b);
  ASSERT_EQ(d4.c, d2.c);
}

TEST_F(FnPatchTest, PatchNonOriginalStruct) {
  // This test is someone misleading, operating on integers.
  // It should be rewritten to operate on character arrays or something.
  // Treating structs as char* means the atomic unit of diffing
  // is a byte, not an integer, so diffs may be on an intra-integer level.

  struct Data {
    int a;
    int b;
    int c;
  };

  auto d1 = Data{.a = 1, .b = 2, .c = 3};
  auto d2 = Data{.a = 1, .b = 4, .c = 3};
  // The difference between d1 and d2 is that b was changed to 4.
  auto delta = badiff::Delta::Make( //
      (char *)&d1, sizeof(d1),      //
      (char *)&d2, sizeof(d2));

  auto d3 = Data{.a = 5, .b = 6, .c = 7};
  auto d4 = Data{.a = 0, .b = 0, .c = 0};
  // Apply that difference to d3, storing in d4.
  delta->Apply((char *)&d3, (char *)&d4);

  // d4 should only differ from d3 in that b was changed to 4.
  ASSERT_EQ(d4.a, 5);
  ASSERT_EQ(d4.b, 4);
  ASSERT_EQ(d4.c, 7);
}
