#ifndef BADIFF_OP_HPP_
#define BADIFF_OP_HPP_

#include <badiff/bytes.hpp>
#include <cstdint>
#include <memory>


namespace badiff {

class Op {
public:
	using Value = Bytes;
	using Length = std::size_t;


	enum Type {
		// The end of a diff.
		STOP,
		// Skip bytes from the original.
		DELETE,
		// Append bytes to the target.
		INSERT,
		// Copy bytes from the original to the target.
		NEXT,
	};

	Op();
	Op(Type&, Length, Value);

	Op(const Op&) = default;
	Op(Op&&) = default;
	Op& operator=(const Op&) = default;
	Op& operator=(Op&&) = default;

private:
	Type type_;
	Length length_;
	Value value_;
};

}

#endif /* BADIFF_OP_HPP_ */
