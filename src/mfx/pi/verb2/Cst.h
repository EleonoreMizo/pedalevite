/*****************************************************************************

        Cst.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_verb2_Cst_HEADER_INCLUDED)
#define mfx_pi_verb2_Cst_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace verb2
{



class Cst
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static constexpr int _max_predelay_time_ms = 500; // ms
	static constexpr int _max_er_duration_ms   = 500; // ms



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Cst ()                               = delete;
	               Cst (const Cst &other)               = delete;
	               Cst (Cst &&other)                    = delete;
	Cst &          operator = (const Cst &other)        = delete;
	Cst &          operator = (Cst &&other)             = delete;
	bool           operator == (const Cst &other) const = delete;
	bool           operator != (const Cst &other) const = delete;

}; // class Cst



}  // namespace verb2
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/verb2/Cst.hpp"



#endif   // mfx_pi_verb2_Cst_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
