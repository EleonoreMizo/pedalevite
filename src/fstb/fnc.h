/*****************************************************************************

        fnc.h
        Author: Laurent de Soras, 2010

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_fnc_HEADER_INCLUDED)
#define	fstb_fnc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>
#include <string>

#include <cstddef>
#include <cstdint>
#include <cstdio>



namespace fstb
{



template <class T>
inline constexpr int     sgn (T x) noexcept;
template <class T>
inline constexpr T       limit (T x, T mi, T ma) noexcept;
template <class T>
inline constexpr void    sort_2_elt (T &mi,T &ma, T a, T b) noexcept;
template <class T>
inline constexpr bool    is_pow_2 (T x) noexcept;
inline double  round (double x) noexcept;
inline float   round (float x) noexcept;
inline int     round_int (float x) noexcept;
inline int     round_int (double x) noexcept;
inline int     round_int_accurate (double x) noexcept;
inline int64_t round_int64 (double x) noexcept;
inline int     floor_int (float x) noexcept;
inline int     floor_int (double x) noexcept;
inline int     floor_int_accurate (double x) noexcept;
inline int64_t floor_int64 (double x) noexcept;
inline int     ceil_int (double x) noexcept;
template <class T>
inline int     trunc_int (T x) noexcept;
template <class T>
inline int     conv_int_fast (T x) noexcept;
template <class T>
inline constexpr bool    is_null (T val, T eps = T (1e-9)) noexcept;
template <class T>
inline constexpr bool    is_eq (T v1, T v2, T eps = T (1e-9)) noexcept;
template <class T>
inline constexpr bool    is_eq_rel (T v1, T v2, T tol = T (1e-6)) noexcept;
inline constexpr bool    is_eq_ulp (float v1, float v2, int32_t tol = 1) noexcept;
inline int     get_prev_pow_2 (uint32_t x) noexcept;
inline int     get_next_pow_2 (uint32_t x) noexcept;
inline constexpr double  sinc (double x) noexcept;
inline double  pseudo_exp (double x, double c) noexcept;
inline double  pseudo_log (double y, double c) noexcept;
template <class T, int S>
inline constexpr T       sshift_l (T x) noexcept;
template <class T, int S>
inline constexpr T       sshift_r (T x) noexcept;
template <typename T>
inline constexpr T       sra_ceil (T x, int s) noexcept;
template <typename T>
inline constexpr T       div_ceil (T num, T den) noexcept;
template <class T>
inline constexpr T       sq (T x) noexcept;
template <class T>
inline constexpr T       cube (T x) noexcept;
template <class T, class U>
inline constexpr T       ipow (T x, U n) noexcept;
template <class T, class U>
inline constexpr T       ipowp (T x, U n) noexcept;
template <int N, class T>
inline constexpr T       ipowpc (T x) noexcept;
template <class T>
inline constexpr T       rcp_uint (int x) noexcept;
template <class T>
inline constexpr T       lerp (T v0, T v1, T p) noexcept;
template <class T>
inline constexpr T       find_extremum_pos_parabolic (T r1, T r2, T r3) noexcept;

template <class T>
inline constexpr T       rotl (T x, int k) noexcept;
template <class T>
inline constexpr T       rotr (T x, int k) noexcept;

template <std::size_t N, typename T>
constexpr std::array <T, N> make_array (const T &init_val);

template <class T>
inline T       read_unalign (const void *ptr) noexcept;
template <class T>
inline void    write_unalign (void *ptr, T val) noexcept;
template <typename T>
inline void    copy_no_overlap (T * fstb_RESTRICT dst_ptr, const T * fstb_RESTRICT src_ptr, int nbr_elt) noexcept;

void           conv_to_lower_case (std::string &str);

int            snprintf4all (char *out_0, size_t size, const char *format_0, ...);
FILE *         fopen_utf8 (const char *filename_0, const char *mode_0);

template <typename T>
inline bool    is_ptr_align_nz (const T *ptr, int a = 16) noexcept;



}	// namespace fstb



#include "fstb/fnc.hpp"



#endif	// fstb_fnc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
