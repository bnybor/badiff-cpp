#include <badiff/op.hpp>

namespace badiff {

Op::Op() :
		type_(STOP), length_(0), value_(nullptr) {
}

Op::Op(Type type, Length length, Value value) :
		type_(type), length_(length), value_(value) {
}

void Op::Serialize(std::ostream &out) {
	std::size_t n = length_;
	n <<= 2;
	n |= (std::size_t) type_;
	n <<= 1;
	n |= (value_ ? 1 : 0);
	out << n;
	if (value_) {
		for (std::size_t i = 0; i < length_; ++i) {
			out << value_[i];
		}
	}
}
void Op::Deserialize(std::istream &in) {
	std::size_t n;
	in >> n;
	bool has_value = ((1 & n) != 0);
	n >>= 1;
	type_ = Type(0x3 & n);
	n >>= 2;
	length_ = n;
	if (has_value) {
		value_ = Bytes(new Byte[length_]);
		for (std::size_t i = 0; i < length_; ++i) {
			in >> value_[i];
		}
	} else {
		value_ = nullptr;
	}
}

const Op::Type& Op::GetType() const {
	return type_;
}
const Op::Length& Op::GetLength() const {
	return length_;
}
const Op::Value& Op::GetValue() const {
	return value_;
}

Op::Type& Op::MutableType() {
	return type_;
}
Op::Length& Op::MutableLength() {
	return length_;
}
Op::Value& Op::MutableValue() {
	return value_;
}

}
