#ifndef BADIFF_OP_HPP_
#define BADIFF_OP_HPP_

#include <cstdint>
#include <memory>

namespace badiff {

class Op {
public:
	using RunLength = std::size_t;
	using RunData = std::shared_ptr<std::uint8_t[]>;

	enum class Type {
		STOP, DELETE, INSERT, NEXT,
	};

	Op();
	Op(Type&, RunLength);

	Op(const Op&) = default;
	Op(Op&&) = default;
	Op& operator=(const Op&) = default;
	Op& operator=(Op&&) = default;

	Type type_;
	RunLength length_;
	RunData data_;
};

}

#endif /* BADIFF_OP_HPP_ */
