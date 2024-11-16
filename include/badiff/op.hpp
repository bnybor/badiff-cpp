#ifndef BADIFF_OP_HPP_
#define BADIFF_OP_HPP_

#include <cstdint>
#include <memory>

#include <badiff/bytes.hpp>
#include <badiff/io/serialized.hpp>

namespace badiff {

class Op : public io::Serialized {
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

	void Serialize(std::ostream& out) const override;
	void Deserialize(std::istream& in) override;


private:
	Type type_;
	Length length_;
	Value value_;
};

}

#endif /* BADIFF_OP_HPP_ */
