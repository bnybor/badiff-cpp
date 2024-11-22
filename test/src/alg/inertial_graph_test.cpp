#include <badiff/alg/inertial_graph.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace badiff;

class InertialGraphTest: public testing::Test {
public:
	virtual ~InertialGraphTest() = default;

};

TEST_F(InertialGraphTest, TestIdentity) {
	alg::InertialGraph graph;
	std::string hello = "hello";

	ByteArray original(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), original.get());

	ByteArray target(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), target.get());

	graph.Compute(original.get(), hello.size(), target.get(), hello.size());

	auto op_queue = graph.MakeOpQueue();

	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string(">5"));
}

TEST_F(InertialGraphTest, TestHelloWorld) {
	alg::InertialGraph graph;
	std::string hello = "hello";
	std::string world = "world";

	ByteArray original(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), original.get());

	ByteArray target(new Byte[world.size()]);
	std::copy(world.begin(), world.end(), target.get());

	graph.Compute(original.get(), hello.size(), target.get(), world.size());

	auto op_queue = graph.MakeOpQueue();

	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("+5-5"));
}

TEST_F(InertialGraphTest, TestHelloAB) {
	alg::InertialGraph graph;
	std::string hello = "ab";
	std::string world = "bb";

	ByteArray original(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), original.get());

	ByteArray target(new Byte[world.size()]);
	std::copy(world.begin(), world.end(), target.get());

	graph.Compute(original.get(), hello.size(), target.get(), world.size());

	auto op_queue = graph.MakeOpQueue();

	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("+2-2"));
}
TEST_F(InertialGraphTest, TestHelloA8B8) {
	alg::InertialGraph graph;
	std::string hello = "aaaaaaaabbbbbbbb";
	std::string world = "bbbbbbbbbbbbbbbb";

	ByteArray original(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), original.get());

	ByteArray target(new Byte[world.size()]);
	std::copy(world.begin(), world.end(), target.get());

	graph.Compute(original.get(), hello.size(), target.get(), world.size());

	auto op_queue = graph.MakeOpQueue();

	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("+10-10>6"));
}
