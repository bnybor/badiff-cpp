#include <badiff/op.hpp>

namespace badiff {

Op::Op() : type_(STOP), length_(0), value_(nullptr) {}

Op::Op(Type type, Length length, Value value)
    : type_(type), length_(length), value_(std::move(value)) {}

Op::Op(const Op &other) : type_(other.GetType()), length_(other.GetLength()) {
  if (other.GetValue()) {
    value_.reset(new Byte[other.GetLength()]);
    std::copy(other.GetValue().get(),
              other.GetValue().get() + other.GetLength(), value_.get());
  }
}

void Op::Serialize(std::ostream &out) {
  std::size_t n = length_;
  n <<= 2;
  n |= (std::size_t)type_;
  n <<= 1;
  n |= (value_ ? 1 : 0);
  char buf[8];
  buf[7] = (n & 0x7f);
  n >>= 7;
  std::size_t i = 7;
  while (n) {
    --i;
    buf[i] = (n & 0x7f);
    buf[i] |= 0x80;
    n >>= 7;
  }
  out.write(buf + i, 8 - i);
  if (value_) {
    out.write((char*)value_.get(), length_);
  }
}
void Op::Deserialize(std::istream &in) {
  std::size_t n = 0;
  std::uint8_t c;
  do {
    in.read((char*)&c, 1);
    n <<= 7;
    n |= (c & 0x7f);
  } while (c & 0x80);
  bool has_value = ((1 & n) != 0);
  n >>= 1;
  type_ = Type(0x3 & n);
  n >>= 2;
  length_ = n;
  if (has_value) {
    value_ = ByteArray(new Byte[length_]);
    in.read((char*)value_.get(), length_);
  } else {
    value_ = nullptr;
  }
}

const Op::Type &Op::GetType() const { return type_; }
const Op::Length &Op::GetLength() const { return length_; }
const Op::Value &Op::GetValue() const { return value_; }

Op::Type &Op::MutableType() { return type_; }
Op::Length &Op::MutableLength() { return length_; }
Op::Value &Op::MutableValue() { return value_; }

} // namespace badiff
