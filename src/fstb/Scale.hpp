/*****************************************************************************

        Scale.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_Scale_CODEHEADER_INCLUDED)
#define fstb_Scale_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#if fstb_ARCHI == fstb_ARCHI_ARM
	#include <arm_neon.h>
#endif
#if defined (_MSC_VER)
	#include <intrin.h>
#endif

#include <type_traits>

#include <cstdint>



namespace fstb
{



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/


// Defines an integer type with double capacity
// Default is not defined. Only specializations can be used.
template <typename T>
struct Scale_Int2x { };

template <>
struct Scale_Int2x <int8_t> { typedef int16_t Ty; };
template <>
struct Scale_Int2x <int16_t> { typedef int32_t Ty; };
template <>
struct Scale_Int2x <int32_t> { typedef int64_t Ty; };

template <>
struct Scale_Int2x <uint8_t> { typedef uint16_t Ty; };
template <>
struct Scale_Int2x <uint16_t> { typedef uint32_t Ty; };
template <>
struct Scale_Int2x <uint32_t> { typedef uint64_t Ty; };



// Floating point and other types
template <typename T, int LL2, bool INT_FLAG>
class Scale_Internal
{
public:
	static constexpr fstb_FORCEINLINE T mul (T a, T b) noexcept
	{
		return a *= b;
	}
};

// Specialization for integer types
template <typename T, int LL2>
class Scale_Internal <T, LL2, true>
{
public:
	static constexpr fstb_FORCEINLINE T mul (T a, T b) noexcept
	{
		typedef typename Scale_Int2x <T>::Ty T2;
		return T ((T2 (a) * T2 (b)) >> LL2);
	}
};

#if fstb_ARCHI == fstb_ARCHI_ARM

template <>
class Scale_Internal <int32_t, 31, true>
{
public:
	static fstb_FORCEINLINE int32_t mul (int32_t a, int32_t b) noexcept
	{
		const auto     aa = vdup_n_s32 (a);
		const auto     bb = vdup_n_s32 (b);
		const auto     rr = vqrdmulh_s32 (aa, bb);

		return vget_lane_s32 (rr, 0);
	}
};
template <>
class Scale_Internal <int32_t, 30, true>
{
public:
	static fstb_FORCEINLINE int32_t mul (int32_t a, int32_t b) noexcept
	{
		return Scale_Internal <int32_t, 31, true>::mul (a, b) << 1;
	}
};

#elif fstb_ARCHI == fstb_ARCHI_X86 && defined (_MSC_VER)

template <int LL2>
class Scale_Internal <uint32_t, LL2, true>
{
public:
	static fstb_FORCEINLINE uint32_t mul (uint32_t a, uint32_t b) noexcept
	{
		return uint32_t (__emulu (a, b) >> LL2);
	}
};

#	if fstb_WORD_SIZE == 64

template <>
class Scale_Internal <uint64_t, 64, true>
{
public:
	static fstb_FORCEINLINE uint64_t mul (uint64_t a, uint64_t b) noexcept
	{
		return __umulh (a, b);
	}
};

#	endif // fstb_WORD_SIZE 64

#endif // ARM, X86, _MSC_VER




/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int LL2>
template <typename T>
T	Scale <LL2>::mul (T a, T b) noexcept
{
	return Scale_Internal <T, LL2, std::is_integral <T>::value>::mul (a, b);
}



}  // namespace fstb



#endif   // fstb_Scale_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
