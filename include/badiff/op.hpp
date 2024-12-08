#ifndef BADIFF_OP_HPP_
#define BADIFF_OP_HPP_

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

namespace badiff {
class Op;
}

#include <badiff/bytes.hpp>

namespace badiff {

class Op {
public:
  enum Type {
    // The end of a diff.
    STOP = 0,
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

  Op(const badiff::Op &other);
  Op(Op &&) = default;
  Op &operator=(const Op &) = delete;
  Op &operator=(Op &&) = default;

  void Serialize(std::ostream &out) const;
  void Deserialize(std::istream &in);
  void Apply(std::istream &original, std::ostream &target) const;

  const Type &GetType() const;
  const Length &GetLength() const;
  const Value &GetValue() const;

  Type &MutableType();
  Length &MutableLength();
  Value &MutableValue();

private:
  Type type_;
  Length length_;
  Value value_;
};

} // namespace badiff

#endif /* BADIFF_OP_HPP_ */
