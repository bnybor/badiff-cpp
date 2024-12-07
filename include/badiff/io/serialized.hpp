#ifndef BADIFF_IO_SERIALIZED_HPP_
#define BADIFF_IO_SERIALIZED_HPP_

#include <istream>
#include <ostream>

namespace badiff {
namespace io {

class Serialized {
public:
  virtual ~Serialized() = default;

  virtual void Serialize(std::ostream &out) = 0;
  virtual void Deserialize(std::istream &in) = 0;
};
} // namespace io
} // namespace badiff

#endif /* BADIFF_IO_SERIALIZED_HPP_ */
