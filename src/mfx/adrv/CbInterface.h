/*****************************************************************************

        CbInterface.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_adrv_CbInterface_HEADER_INCLUDED)
#define mfx_adrv_CbInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace adrv
{



class CbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               CbInterface ()                         = default;
	               CbInterface (const CbInterface &other) = default;
	               CbInterface (CbInterface &&other)      = default;

	virtual        ~CbInterface ()                        = default;

	virtual CbInterface &
	               operator = (const CbInterface &other)  = default;
	virtual CbInterface &
	               operator = (CbInterface &&other)       = default;

	void           process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl);
	void           notify_dropout ();
	void           request_exit ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_process_block (float * const * dst_arr, const float * const * src_arr, int nbr_spl) = 0;
	virtual void   do_notify_dropout () = 0;
	virtual void   do_request_exit () = 0;



}; // class CbInterface



}  // namespace adrv
}  // namespace mfx



//#include "mfx/adrv/CbInterface.hpp"



#endif   // mfx_adrv_CbInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
