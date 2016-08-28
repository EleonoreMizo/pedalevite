/*****************************************************************************

        Phase90Interface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_phase1_Phase90Interface_HEADER_INCLUDED)
#define mfx_pi_phase1_Phase90Interface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace pi
{
namespace phase1
{



class Phase90Interface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual        ~Phase90Interface () = default;

	int            get_nbr_coefs () const;
	void           set_coefs (const double coef_arr []);
	void           clear_buffers ();
	void           process_block (float dst_0_ptr [], float dst_1_ptr [], const float src_ptr [], int nbr_spl);
	void           process_sample (float &dst_0, float &dst_1, float src);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual int    do_get_nbr_coefs () const = 0;
	virtual void   do_set_coefs (const double coef_arr []) = 0;
	virtual void   do_clear_buffers () = 0;
	virtual void   do_process_block (float dst_0_ptr [], float dst_1_ptr [], const float src_ptr [], int nbr_spl) = 0;
	virtual void   do_process_sample (float &dst_0, float &dst_1, float src) = 0;



}; // class Phase90Interface



}  // namespace phase1
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/phase1/Phase90Interface.hpp"



#endif   // mfx_pi_phase1_Phase90Interface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
