#ifndef BADIFF_IO_SERIALIZED_HPP_
#define BADIFF_IO_SERIALIZED_HPP_

#include <istream>
#include <ostream>

namespace badiff {
namespace io {

class Serialized {
public:
	virtual ~Serialized() = default;

	void Serialize(std::ostream& out) const = 0;
	void Deserialize(std::istream& in) = 0;
};
}
}



#endif /* BADIFF_IO_SERIALIZED_HPP_ */
