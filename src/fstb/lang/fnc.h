/*****************************************************************************

        fnc.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (fstb_lang_fnc_HEADER_INCLUDED)
#define fstb_lang_fnc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/lang/NonConst.h"



namespace fstb
{
namespace lang
{



template <typename TConstReturn, class TObj, typename... TArgs>
typename NonConst <TConstReturn>::type like_const_version (TObj const* obj, TConstReturn (TObj::*memFun) (TArgs...) const, TArgs&&... args);



}  // namespace lang
}  // namespace fstb



#include "fstb/lang/fnc.hpp"



#endif   // fstb_lang_fnc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
