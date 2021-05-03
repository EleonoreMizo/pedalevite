/*****************************************************************************

        AntisatAtanh.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_AntisatAtanh_HEADER_INCLUDED)
#define mfx_dsp_va_AntisatAtanh_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



class AntisatAtanh
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static fstb_FORCEINLINE void
	               eval (float &y, float &dy, float x) noexcept;
	static fstb_FORCEINLINE float
	               eval_inv (float y) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static fstb_FORCEINLINE float
	               tanh_fast (float x) noexcept;
	static fstb_FORCEINLINE float
	               atanh_fast (float x) noexcept;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               AntisatAtanh ()                               = delete;
	               AntisatAtanh (const AntisatAtanh &other)      = delete;
	               AntisatAtanh (AntisatAtanh &&other)           = delete;
	               ~AntisatAtanh ()                              = delete;
	AntisatAtanh & operator = (const AntisatAtanh &other)        = delete;
	AntisatAtanh & operator = (AntisatAtanh &&other)             = delete;
	bool           operator == (const AntisatAtanh &other) const = delete;
	bool           operator != (const AntisatAtanh &other) const = delete;

}; // class AntisatAtanh



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/AntisatAtanh.hpp"



#endif   // mfx_dsp_va_AntisatAtanh_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
