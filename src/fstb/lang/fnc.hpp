/*****************************************************************************

        fnc.hpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (fstb_lang_fnc_CODEHEADER_INCLUDED)
#define fstb_lang_fnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <utility>



namespace fstb
{
namespace lang
{



// Usage: return like_const_version (this, &ThisType::fnc, param1, param2...);
template <typename TConstReturn, class TObj, typename... TArgs>
typename NonConst <TConstReturn>::type	like_const_version (TObj const* obj, TConstReturn (TObj::*memFun) (TArgs...) const, TArgs&&... args)
{
	return const_cast <typename NonConst <TConstReturn>::type> ((obj->*memFun) (std::forward <TArgs> (args)...));
}



}  // namespace lang
}  // namespace fstb



#endif   // fstb_lang_fnc_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
