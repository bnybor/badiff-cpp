/*
Copyright 2025 Robyn Kirkman

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <badiff/op.hpp>

namespace badiff {

Op::Op() : type_(STOP), length_(0), value_(nullptr) {}

Op::Op(Type type, Length length, std::unique_ptr<char[]> value)
    : type_(type), length_(length), value_(std::move(value)) {}

Op::Op(Type type, const std::string &value) : type_(type) {
  length_ = value.size();
  value_.reset(new char[length_]);
  std::copy(value.begin(), value.end(), value_.get());
}

Op::Op(const Op &other) : type_(other.GetType()), length_(other.GetLength()) {
  if (other.GetValue()) {
    value_.reset(new char[other.GetLength()]);
    std::copy(other.GetValue().get(),
              other.GetValue().get() + other.GetLength(), value_.get());
  }
}

Op &Op::operator=(const Op &op) { return *this = Op(op); }

void Op::Serialize(std::ostream &out) const {
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
    out.write((char *)value_.get(), length_);
  }
}
void Op::Deserialize(std::istream &in) {
  std::size_t n = 0;
  char c;
  do {
    in.read(&c, 1);
    n <<= 7;
    n |= (c & 0x7f);
  } while (c & 0x80);
  bool has_value = ((1 & n) != 0);
  n >>= 1;
  type_ = Type(0x3 & n);
  n >>= 2;
  length_ = n;
  if (has_value) {
    value_ = std::unique_ptr<char[]>(new char[length_]);
    in.read(value_.get(), length_);
  } else {
    value_ = nullptr;
  }
}

void Op::Apply(std::istream &original, std::ostream &target) const {
  switch (type_) {
  case DELETE:
    original.ignore(length_);
    break;
  case INSERT:
    target.write(value_.get(), length_);
    break;
  case NEXT:
    char buf[length_];
    original.read(buf, length_);
    target.write(buf, length_);
    break;
  }
}

bool Op::operator==(const Op &rhs) const {
  if (type_ != rhs.type_)
    return false;
  if (length_ != rhs.length_)
    return false;
  if (value_) {
    if (!rhs.value_)
      return false;
    return std::equal(value_.get(), value_.get() + length_, rhs.value_.get());
  } else {
    return rhs.value_ == nullptr;
  }
}

const Op::Type &Op::GetType() const { return type_; }
const Op::Length &Op::GetLength() const { return length_; }
const std::unique_ptr<char[]> &Op::GetValue() const { return value_; }

Op::Type &Op::MutableType() { return type_; }
Op::Length &Op::MutableLength() { return length_; }
std::unique_ptr<char[]> &Op::MutableValue() { return value_; }

} // namespace badiff
