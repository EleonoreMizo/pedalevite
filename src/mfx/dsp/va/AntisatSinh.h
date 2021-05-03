/*****************************************************************************

        AntisatSinh.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_AntisatSinh_HEADER_INCLUDED)
#define mfx_dsp_va_AntisatSinh_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"



namespace mfx
{
namespace dsp
{
namespace va
{



class AntisatSinh
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
	               sinh_fast (float x) noexcept;
	static fstb_FORCEINLINE float
	               asinh_fast (float x) noexcept;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               AntisatSinh ()                               = delete;
	               AntisatSinh (const AntisatSinh &other)       = delete;
	               AntisatSinh (AntisatSinh &&other)            = delete;
	               ~AntisatSinh ()                              = delete;
	AntisatSinh &  operator = (const AntisatSinh &other)        = delete;
	AntisatSinh &  operator = (AntisatSinh &&other)             = delete;
	bool           operator == (const AntisatSinh &other) const = delete;
	bool           operator != (const AntisatSinh &other) const = delete;

}; // class AntisatSinh



}  // namespace va
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/va/AntisatSinh.hpp"



#endif   // mfx_dsp_va_AntisatSinh_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
