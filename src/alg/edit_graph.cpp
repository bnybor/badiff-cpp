#include <badiff/alg/edit_graph.hpp>

#include <algorithm>

namespace badiff {

namespace alg {

void EditGraph::Compute(const Byte *original, std::size_t original_length,
		const Byte *target, std::size_t target_length) {

	std::size_t xlen = original_length + 1;
	std::size_t ylen = target_length + 1;

	std::unique_ptr<Byte[]> xval(new Byte[xlen]);
	std::unique_ptr<Byte[]> yval(new Byte[ylen]);

	xval[0] = 0;
	yval[0] = 0;
	std::copy(original, original + original_length, &xval[1]);
	std::copy(target, target + target_length, &yval[1]);

	// The best operation to enter a comparison.
	Op::Type flags[xlen * ylen];
	// The path length of the best operation to enter a comparison.
	std::size_t lengths[xlen * ylen];

	std::fill(flags, flags + xlen * ylen, Op::STOP);
	std::fill(lengths, lengths + xlen * ylen, 0);

	for (std::size_t y = 0; y < target_length + 1; ++y) {
		for (std::size_t x = 0; x < original_length + 1; ++x) {
			if (x == 0 && y == 0)
				continue;
			std::size_t pos = x + y * xlen;
			if (x > 0 && y > 0 && xval[x] == yval[y]) {
				flags[pos] = Op::NEXT;
				lengths[pos] = (1 + lengths[pos - xlen - 1]);
				continue;
			}
			std::size_t dlen = (x > 0) ? (1 + lengths[pos - 1]) : -1;
			std::size_t ilen = (y > 0) ? (1 + lengths[pos - xlen]) : -1;
			if (dlen <= ilen) {
				flags[pos] = Op::DELETE;
				lengths[pos] = dlen;
			} else {
				flags[pos] = Op::INSERT;
				lengths[pos] = ilen;
			}
		}
	}

	std::vector<Op> ret;
	std::vector<Byte> buf;
	Op::Type op = Op::STOP;
	std::size_t run = 0;

	int pos = xlen * ylen - 1;
	while (pos > 0) {
		Op::Type fop = flags[pos];
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

	op_queue_.resize(ret.size());
	std::move(ret.rbegin(), ret.rend(), op_queue_.begin());
}

std::unique_ptr<q::OpQueue> EditGraph::MakeOpQueue() const {
	std::vector<Op> queue;
	for (const auto& op : op_queue_) {
		ByteArray value;
		if (op.GetValue()) {
			value.reset(new Byte[op.GetLength()]);
			std::copy(op.GetValue().get(), op.GetValue().get() + op.GetLength(), value.get());
		}
		queue.push_back(Op(op.GetType(), op.GetLength(), std::move(value))); // @suppress("Ambiguous problem")
	}
	return std::unique_ptr<q::OpQueue>(new q::OpQueue(std::move(queue)));
}

}

}