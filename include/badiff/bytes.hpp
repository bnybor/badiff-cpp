#ifndef BADIFF_BYTES_HPP_
#define BADIFF_BYTES_HPP_

#include <cstdint>
#include <memory>

namespace badiff {

using Byte = std::uint8_t;
using ByteArray = std::unique_ptr<Byte[]>;

} // namespace badiff

#endif /* BADIFF_BYTES_HPP_ */
