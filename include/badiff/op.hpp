#ifndef BADIFF_OP_HPP_
#define BADIFF_OP_HPP_

#include <cstdint>
#include <memory>

namespace badiff {
class Op;
}

#include <badiff/bytes.hpp>
#include <badiff/io/serialized.hpp>

namespace badiff {

class Op : public io::Serialized {
public:
	enum Type {
		// The end of a diff.
		NONE = 0,
		// Skip bytes from the original.
		DELETE = 1,
		// Append bytes to the target.
		INSERT = 2,
		// Copy bytes from the original to the target.
		NEXT = 3,
	};
	using Value = ByteArray;
	using Length = std::size_t;


	Op();
	Op(Type, Length = 0, Value = nullptr);

	Op(const Op&) = default;
	Op(Op&&) = default;
	Op& operator=(const Op&) = default;
	Op& operator=(Op&&) = default;

	void Serialize(std::ostream& out) override;
	void Deserialize(std::istream& in) override;

	const Type& GetType() const;
	const Length& GetLength() const;
	const Value& GetValue() const;

	Type& MutableType();
	Length& MutableLength();
	Value& MutableValue();

private:
	Type type_;
	Length length_;
	Value value_;
};

}

#endif /* BADIFF_OP_HPP_ */
