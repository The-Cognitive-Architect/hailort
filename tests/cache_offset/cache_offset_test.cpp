/** Distributed under the MIT license (https://opensource.org/licenses/MIT) */
#include "core_op/resource_manager/cache_offset.hpp"

#include <cstdint>
#include <iostream>
#include <limits>

static uint32_t reference(uint32_t offset, int32_t delta, uint32_t length)
{
    // Independent bounded repeated-wrap reference for the exhaustive small cases.
    auto value = static_cast<int64_t>(offset) + delta;
    while (value < 0) { value += length; }
    while (value >= length) { value -= length; }
    return static_cast<uint32_t>(value);
}

int main()
{
    struct Case { uint32_t offset; int32_t delta; uint32_t length; uint32_t expected; };
    const auto max_u32 = std::numeric_limits<uint32_t>::max();
    const auto min_i32 = std::numeric_limits<int32_t>::min();
    const auto max_i32 = std::numeric_limits<int32_t>::max();
    const Case boundaries[] = {
        {0, -1, 1536, 1535}, {1535, 1, 1536, 0}, {7, -8, 384, 383},
        {0, min_i32, 1536, 1024}, {0, min_i32, max_u32, 2147483647U},
        {max_u32 - 1, max_i32, max_u32, 2147483646U},
        {max_u32 - 1, min_i32, max_u32, 2147483646U}, {0, min_i32, 1, 0}
    };
    uint64_t checked = 0;
    for (const auto &item : boundaries) {
        const auto actual = hailort::detail::advance_cache_offset(item.offset, item.delta, item.length);
        if (actual != item.expected) {
            std::cerr << "Boundary mismatch: offset=" << item.offset << " delta=" << item.delta
                      << " length=" << item.length << " actual=" << actual << " expected=" << item.expected << '\n';
            return 1;
        }
        ++checked;
    }
    const uint32_t lengths[] = {1, 3, 128, 256, 384, 512, 768, 1536};
    for (const auto length : lengths) {
        const auto span = static_cast<int32_t>(length * 2);
        for (uint32_t offset = 0; offset < length; ++offset) {
            for (int32_t delta = -span; delta <= span; ++delta) {
                const auto actual = hailort::detail::advance_cache_offset(offset, delta, length);
                if (actual != reference(offset, delta, length)) {
                    std::cerr << "Ring mismatch: offset=" << offset << " delta=" << delta << " length=" << length << '\n';
                    return 1;
                }
                ++checked;
            }
        }
    }
    std::cout << "Passed " << checked << " cache-offset cases\n";
    return 0;
}
