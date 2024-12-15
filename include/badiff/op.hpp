#ifndef BADIFF_OP_HPP_
#define BADIFF_OP_HPP_

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

namespace badiff {
class Op;
}

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
  using Length = int;

  Op();
  Op(Type, Length = 1, std::unique_ptr<char[]> = nullptr);
  Op(Type, const std::string &);

  Op(const Op &other);
  Op(Op &&) = default;
  Op &operator=(const Op &);
  Op &operator=(Op &&) = default;

  void Serialize(std::ostream &out) const;
  void Deserialize(std::istream &in);
  void Apply(std::istream &original, std::ostream &target) const;

  const Type &GetType() const;
  const Length &GetLength() const;
  const std::unique_ptr<char[]> &GetValue() const;

  Type &MutableType();
  Length &MutableLength();
  std::unique_ptr<char[]> &MutableValue();

  bool operator==(const Op &) const;

private:
  Type type_;
  Length length_;
  std::unique_ptr<char[]> value_;
};

} // namespace badiff

#endif /* BADIFF_OP_HPP_ */
