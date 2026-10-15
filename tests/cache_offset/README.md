# Cache-offset regression

This test calls the internal arithmetic helper used by `CacheManager`.
It needs only a C++11 compiler and CMake; no device, firmware, protobuf
dependency, or runtime installation is required.

```sh
cmake -S tests/cache_offset -B build/cache-offset -DCMAKE_BUILD_TYPE=Release
cmake --build build/cache-offset --parallel 2
ctest --test-dir build/cache-offset --output-on-failure
```

The old expression adds a signed delta to an unsigned offset. At offset 0,
delta -1 and cache length 1536, unsigned wrap followed by modulo yields
1023 instead of 1535. Widening before the addition and normalizing a
negative remainder preserves mathematical ring behavior. The regression
covers negative and positive wraps, power-of-two and other lengths,
INT32_MIN/INT32_MAX deltas, and UINT32_MAX lengths. It uses the same
production helper, including when compiled with `NDEBUG`.

This is an arithmetic correction. The test does not establish that a
particular DFC-generated graph reaches the failing combination on hardware.
