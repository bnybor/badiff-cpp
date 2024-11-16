#ifndef BADIFF_BYTES_HPP_
#define BADIFF_BYTES_HPP_

#include <memory>
#include <cstdint>

namespace badiff {

using Byte = std::uint8_t;
using ByteArray = std::unique_ptr<Byte[]>;

}



#endif /* BADIFF_BYTES_HPP_ */
