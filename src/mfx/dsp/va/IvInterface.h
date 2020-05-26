/*****************************************************************************

        IvInterface.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_va_IvInterface_HEADER_INCLUDED)
#define mfx_dsp_va_IvInterface_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace dsp
{
namespace va
{



class IvInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               IvInterface ()                               = default;
	               IvInterface (const IvInterface &other)       = default;
	               IvInterface (IvInterface &&other)            = default;
	virtual        ~IvInterface ()                              = default;

	virtual IvInterface &
	               operator = (const IvInterface &other)        = default;
	virtual IvInterface &
	               operator = (IvInterface &&other)             = default;

	virtual void   eval (float &y, float &dy, float x) const = 0;
	virtual float  get_max_step (float x) const = 0;




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



}; // class IvInterface



}  // namespace va
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/va/IvInterface.hpp"



#endif   // mfx_dsp_va_IvInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
