/**
 * Distributed under the MIT license (https://opensource.org/licenses/MIT)
 **/
#ifndef _HAILO_CACHE_OFFSET_HPP_
#define _HAILO_CACHE_OFFSET_HPP_

#include <cassert>
#include <cstdint>

namespace hailort
{
namespace detail
{

// Cache allocation guarantees a nonzero length. Widen before addition so
// negative deltas do not wrap modulo 2^32 before wrapping around the cache.
inline uint32_t advance_cache_offset(uint32_t read_offset, int32_t delta, uint32_t cache_length)
{
    assert(cache_length > 0);
    const auto length = static_cast<int64_t>(cache_length);
    const auto offset = static_cast<int64_t>(read_offset) + static_cast<int64_t>(delta);
    const auto remainder = offset % length;
    return static_cast<uint32_t>(remainder < 0 ? remainder + length : remainder);
}

} // namespace detail
} // namespace hailort

#endif // _HAILO_CACHE_OFFSET_HPP_
