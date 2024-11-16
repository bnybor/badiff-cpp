#ifndef BADIFF_IO_SERIALIZED_HPP_
#define BADIFF_IO_SERIALIZED_HPP_

#include <istream>
#include <ostream>

#include <badiff/bytes.hpp>
namespace badiff {
namespace io {

class Serialized {
public:
	virtual ~Serialized() = default;

	virtual void Serialize(std::ostream& out) = 0;
	virtual void Deserialize(std::istream& in) = 0;
};
}
}



#endif /* BADIFF_IO_SERIALIZED_HPP_ */
