# Firesledge’s Toolbox

A basic toolbox containing essential utility functions and classes.

Subdomains:

- **`lang`**: Some helper functions and classes to workaround some limitations and quirks in the C++ language. Has been massively depopulated since C++11.
- **`msg`**: Lock-free communication systems based on the `conc` library.
- **`txt`**: Various text-processing classes. Most of them handle UTF-8, UTF-16 and Unicode conversions.
- **`util`**: Here are some observer pattern classes. Actually I’d better rename this namespace…

The classes:

- **`AllocAlign`**: standard allocator for aligned memory. A must have when using SIMD processing.
- **`Approx`**: fast approximations of usual mathematic function (sin, log, exp, tan…)
- **`ArrayAlign`**: An aligned array without memory allocation (works on the stack or as static data).
- **`BitField*`**: Tools to handle large bit fields.
- **`CpuId`**: gives information about the CPU on which the program runs.
- **`Crc32`**: a class to compute simple CRC32 (using the 0xEDB88320 polynomial).
- **`DataAlign`**: A helper class to load and save float data and check pointers, their alignment being a bool template parameter.
- **`FixedPoint`**: A class handling 32:32 fixed point data with all required operations.
- **`Hash`**: A fast, reversible 32 and 64-bit bijective hash function.
- **`RndPcgXshRr6432`**: PCG XSH RR 64/32, a fast 32-bit random generator.
- **`RndXoroshiro128p`**: xoroshiro128+, a fast and compact 64-bit random generator.
- **`Scale`**: A way to multiply two numbers whatever their type; integers are considered as fixed point data. This class simplifies template constructions
- **`SingleObj`**: A class to wrap any object with aligned memory. Easier than using a single-element vector.
- **`ToolsSimd`**: Functions abstracting operations using SSE/SSE2 or NEON, depending on the architecture. 
- **`ToolsSse2`**, **`ToolsAvx2`**: provide various functions using SSE2 and AVX2 instructions. These classes were written mainly with 16-bit pixel processing in mind.

**`def`** and **`fnc`** provide miscellaneous functions, macros and constants.

About the random number generators: `Hash` is the fastest one, but only when the datasize is not greater than the architecture word length.
`RndPcgXshRr6432` is probably sligthly better than `RndXoroshiro128p` regarding statistical quality but always slower, particularly on a 32-bit arch.
