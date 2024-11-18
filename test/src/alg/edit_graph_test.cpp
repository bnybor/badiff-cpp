#include <badiff/alg/edit_graph.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace badiff;

class EditGraphTest: public testing::Test {
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

	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string(">5"));
}

TEST_F(EditGraphTest, TestHelloWorld) {
	alg::EditGraph graph;
	std::string hello = "hello";
	std::string world = "world";

	ByteArray original(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), original.get());

	ByteArray target(new Byte[world.size()]);
	std::copy(world.begin(), world.end(), target.get());

	graph.Compute(original.get(), hello.size(), target.get(), world.size());

	auto op_queue = graph.MakeOpQueue();

	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("+3-2>1+1-2"));
}
