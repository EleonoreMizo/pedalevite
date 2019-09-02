/*****************************************************************************

        type_name.h
        Author: Laurent de Soras, 2019

Mostly copied from
https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/20170989#20170989

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_lang_type_name_HEADER_INCLUDED)
#define fstb_lang_type_name_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <ostream>
#include <string>

#ifndef _MSC_VER
#  if __cplusplus < 201103
#    define CONSTEXPR11_TN
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN
#  elif __cplusplus < 201402
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN noexcept
#  else
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN constexpr
#    define NOEXCEPT_TN noexcept
#  endif
#else  // _MSC_VER
#  if _MSC_VER < 1900
#    define CONSTEXPR11_TN
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN
#  elif _MSC_VER < 2000
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN noexcept
#  else
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN constexpr
#    define NOEXCEPT_TN noexcept
#  endif
#endif  // _MSC_VER



namespace fstb
{
namespace lang
{



class static_string
{
public:

	typedef const char * const_iterator;

	template <std::size_t N>
	CONSTEXPR11_TN static_string (const char (&a) [N]) NOEXCEPT_TN
	:	p_ (a)
	,	sz_ (N - 1)
	{
		// Nothing
	}

	CONSTEXPR11_TN static_string (const char *p, std::size_t N) NOEXCEPT_TN
	:	p_ (p)
	,	sz_ (N)
	{
		// Nothing
	}

	CONSTEXPR11_TN const char *
	               data () const NOEXCEPT_TN
	{
		return p_;
	}
	CONSTEXPR11_TN std::size_t
	               size () const NOEXCEPT_TN
	{
		return sz_;
	}

	CONSTEXPR11_TN const_iterator
	               begin () const NOEXCEPT_TN
	{
		return p_;
	}
	CONSTEXPR11_TN const_iterator
	               end () const NOEXCEPT_TN
	{
		return p_ + sz_;
	}

	CONSTEXPR11_TN char operator [] (std::size_t n) const
	{
		return (n < sz_) ? p_ [n] : throw std::out_of_range ("static_string");
	}

	std::string to_str () const
	{
		return std::string (p_, sz_);
	}

private:

	const char * const
	               p_;
	const std::size_t
	               sz_;
};

inline std::ostream &	operator << (std::ostream &os, static_string const &s)
{
    return os.write (s.data (), s.size ());
}



template <class T>
CONSTEXPR14_TN static_string type_name ()
{
#ifdef __clang__
    static_string p = __PRETTY_FUNCTION__;
    return static_string (p.data () + 55, p.size () - 55 - 1);

#elif defined(__GNUC__)
	static_string p = __PRETTY_FUNCTION__;
	#if (__cplusplus < 201402)
	return static_string (p.data () + 60, p.size () - 60 - 1);
	#else
	return static_string (p.data () + 70, p.size () - 70 - 1);
	#endif

#elif defined (_MSC_VER)
	static_string p = __FUNCSIG__;
	return static_string (p.data () + 62, p.size () - 62 - 7);

#endif
}



#undef CONSTEXPR11_TN
#undef CONSTEXPR14_TN
#undef NOEXCEPT_TN



}  // namespace lang
}  // namespace fstb



//#include "fstb/lang/type_name.hpp"



#endif   // fstb_lang_type_name_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
