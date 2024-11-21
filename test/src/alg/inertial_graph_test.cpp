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

	printf("test\n");

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
	std::string hello = "hello friend of mine";
	std::string world = "world friend of mine";

	ByteArray original(new Byte[hello.size()]);
	std::copy(hello.begin(), hello.end(), original.get());

	ByteArray target(new Byte[world.size()]);
	std::copy(world.begin(), world.end(), target.get());

	graph.Compute(original.get(), hello.size(), target.get(), world.size());

	auto op_queue = graph.MakeOpQueue();

	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string("+3-2>1+1-2"));
}

//TEST_F(InertialGraphTest, TestHelloHellish) {
//	alg::InertialGraph graph;
//	std::string hello = "hello world1234567890";
//	std::string world = "hello ish world";
//
//	ByteArray original(new Byte[hello.size()]);
//	std::copy(hello.begin(), hello.end(), original.get());
//
//	ByteArray target(new Byte[world.size()]);
//	std::copy(world.begin(), world.end(), target.get());
//
//	graph.Compute(original.get(), hello.size(), target.get(), world.size());
//
//	auto op_queue = graph.MakeOpQueue();
//
//	ASSERT_EQ(q::OpQueue::SummarizeConsuming(*op_queue), std::string(">2+1>1+8>1-1>6"));
//}
