#include <badiff/opq.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
class CDiffPatchTest : public testing::Test {};

TEST_F(CDiffPatchTest, Identity) {
  const char *hello = "hello";
  const char *world = "world";
  int q_len = strlen(hello) + strlen(world) + 3;
  op_t q[q_len];
  badiff_compute_opq(q, q_len, hello, strlen(hello) + 1, world, strlen(world) + 1);
  char world2[1024];
  badiff_apply_opq(world2, q, hello);
  ASSERT_EQ(strcmp(world, world2), 0);
}

} // namespace