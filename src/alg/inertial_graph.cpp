#include <badiff/alg/inertial_graph.hpp>

#include <algorithm>

namespace badiff {

namespace alg {

namespace {
constexpr inline std::size_t CostOfOperation(Op::Type previous_op,
		Op::Type op) {
	return InertialGraph::TRANSITION_COSTS[previous_op][op];
}
struct Node {
	const std::size_t *previous_delete_cost_;
	const std::size_t *previous_insert_cost_;
	const std::size_t *previous_next_cost_;
	std::size_t delete_cost_;
	std::size_t insert_cost_;
	std::size_t next_cost_;

	std::uint8_t original_;
	std::uint8_t target_;

	Node() {
		previous_delete_cost_ = nullptr;
		previous_insert_cost_ = nullptr;
		previous_next_cost_ = nullptr;
		delete_cost_ = -1;
		insert_cost_ = -1;
		next_cost_ = -1;
	}

	void Compute() {
		if (previous_delete_cost_) {
			delete_cost_ = std::min(delete_cost_,
					*previous_delete_cost_
							+ CostOfOperation(Op::DELETE, Op::DELETE));
			insert_cost_ = std::min(insert_cost_,
					*previous_delete_cost_
							+ CostOfOperation(Op::DELETE, Op::INSERT));
			next_cost_ = std::min(next_cost_,
					*previous_delete_cost_
							+ CostOfOperation(Op::DELETE, Op::NEXT));
		}
		if (previous_insert_cost_) {
			delete_cost_ = std::min(delete_cost_,
					*previous_insert_cost_
							+ CostOfOperation(Op::INSERT, Op::DELETE));
			insert_cost_ = std::min(insert_cost_,
					*previous_insert_cost_
							+ CostOfOperation(Op::INSERT, Op::INSERT));
			next_cost_ = std::min(next_cost_,
					*previous_insert_cost_
							+ CostOfOperation(Op::INSERT, Op::NEXT));
		}
		if (original_ == target_ && previous_next_cost_) {
			delete_cost_ = std::min(delete_cost_,
					*previous_next_cost_
							+ CostOfOperation(Op::NEXT, Op::DELETE));
			insert_cost_ = std::min(insert_cost_,
					*previous_next_cost_
							+ CostOfOperation(Op::NEXT, Op::INSERT));
			next_cost_ = std::min(next_cost_,
					*previous_next_cost_ + CostOfOperation(Op::NEXT, Op::NEXT));
		}

	}
};
}

void InertialGraph::Compute(const Byte *original, std::size_t original_length,
		const Byte *target, std::size_t target_length) {

	/*
	 * Construct a rectangular graph as the edit graph.
	 * The edit graph supports down (insert), right (delete), and
	 * diagonal down-right (next) traversal.  The edit graph
	 * is a digraph without cycles and can be traversed in
	 * O(n^2) complexity and memory.
	 */

	std::size_t xlen = original_length + 1;
	std::size_t ylen = target_length + 1;

	std::unique_ptr<Byte[]> xval(new Byte[xlen]);
	std::unique_ptr<Byte[]> yval(new Byte[ylen]);

	xval[0] = 0;
	yval[0] = 0;
	std::copy(original, original + original_length, &xval[1]);
	std::copy(target, target + target_length, &yval[1]);

	std::vector<std::vector<Node>> nodes(ylen, std::vector<Node>(xlen));
	/*
	 * Actually traverse the graph, using dynamic programming to store
	 * the best path.
	 */

	for (std::size_t y = 0; y < target_length + 1; ++y) {
		for (std::size_t x = 0; x < original_length + 1; ++x) {
			Node &node = nodes[y][x];
			if (x == 0 && y == 0) {
				node.delete_cost_ = 0;
				node.insert_cost_ = 0;
				node.next_cost_ = 0;
			} else if (x == 0) {
				node.insert_cost_ = 0;
				node.next_cost_ = 0;
				node.previous_insert_cost_ = &nodes[y - 1][x].insert_cost_;
			} else if (y == 0) {
				node.delete_cost_ = 0;
				node.next_cost_ = 0;
				node.previous_delete_cost_ = &nodes[y][x - 1].delete_cost_;
			} else { /* x > 0 && y > 0 */
				node.previous_delete_cost_ = &nodes[y - 1][x].insert_cost_;
				node.previous_insert_cost_ = &nodes[y][x - 1].delete_cost_;
				node.previous_next_cost_ = &nodes[y - 1][x - 1].next_cost_;
			}
			node.original_ = original[x];
			node.target_ = target[y];
			node.Compute();
		}
	}

	std::vector<Op> ret;
	std::vector<Byte> buf;
	Op::Type op = Op::STOP;
	std::size_t run = 0;

	/*
	 * Traverse backward from lower right corner to upper left corner.
	 * This gives the edit script in reverse order.
	 */

	int pos = xlen * ylen - 1;
	while (pos > 0) {
		Op::Type fop = best_op[pos];
		if (op != Op::STOP && op != fop) {
			ByteArray data;
			if (op == Op::INSERT || op == Op::DELETE) {
				auto *rdata = &buf[0];
				data.reset(new Byte[buf.size()]);
				for (std::size_t i = 0; i < buf.size(); ++i) {
					data[buf.size() - i - 1] = rdata[i];
				}
			}
			ret.push_back(Op(op, run, std::move(data))); // @suppress("Ambiguous problem")
			run = 0;
			buf.clear();
		}
		op = fop;
		run++;
		if (op == Op::INSERT) {
			buf.push_back(yval[pos / xlen]);
			pos -= xlen;
		}
		if (op == Op::DELETE) {
			buf.push_back(xval[pos % xlen]);
			pos -= 1;
		}
		if (op == Op::NEXT) {
			pos -= xlen + 1;
		}
	}
	if (op != Op::STOP) {
		ByteArray data;
		if (op == Op::INSERT || op == Op::DELETE) {
			auto *rdata = &buf[0];
			data.reset(new Byte[buf.size()]);
			for (std::size_t i = 0; i < buf.size(); ++i) {
				data[buf.size() - i - 1] = rdata[i];
			}
		}
		ret.push_back(Op(op, run, std::move(data))); // @suppress("Ambiguous problem")
	}

	/*
	 * Finally, reverse the edit script and store it.
	 */
	op_queue_.resize(ret.size());
	std::move(ret.rbegin(), ret.rend(), op_queue_.begin());
}

std::unique_ptr<q::OpQueue> InertialGraph::MakeOpQueue() const {
	/*
	 * Copy the edit script and return it as an OpQueue.
	 */
	std::vector<Op> queue;
	for (const auto &op : op_queue_) {
		ByteArray value;
		if (op.GetValue()) {
			value.reset(new Byte[op.GetLength()]);
			std::copy(op.GetValue().get(), op.GetValue().get() + op.GetLength(),
					value.get());
		}
		queue.push_back(Op(op.GetType(), op.GetLength(), std::move(value))); // @suppress("Ambiguous problem")
	}
	return std::unique_ptr<q::OpQueue>(new q::OpQueue(std::move(queue)));
}

}

}
