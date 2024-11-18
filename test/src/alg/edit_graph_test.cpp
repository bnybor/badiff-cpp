#include <badiff/alg/edit_graph.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace badiff;

class EditGraphTest : public testing::Test {
public:
	virtual ~EditGraphTest() = default;


};

TEST_F(EditGraphTest, TestIdentity) {
	alg::EditGraph graph;
	std::string hello = "hello";

	ByteArray original(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), original.get());

	ByteArray target(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), target.get());

	graph.Compute(original.get(), hello.size(), target.get(), hello.size());

	auto op_queue = graph.MakeOpQueue();

	ASSERT_FALSE(op_queue->IsEmpty());
	Op op = op_queue->PopFront();
	ASSERT_EQ(op.GetType(), Op::NEXT);
	ASSERT_EQ(op.GetLength(), 5);
	ASSERT_TRUE(op_queue->IsEmpty());

}
