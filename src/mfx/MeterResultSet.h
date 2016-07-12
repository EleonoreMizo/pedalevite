/*****************************************************************************

        MeterResultSet.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_MeterResultSet_HEADER_INCLUDED)
#define mfx_MeterResultSet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/Cst.h"
#include "mfx/Dir.h"
#include "mfx/MeterResult.h"

#include <array>
#include <atomic>



namespace mfx
{



class MeterResultSet
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class Side
	{
	public:
		bool           check_signal_clipping ();

		std::array <MeterResult, Cst::_nbr_chn_inout>
		               _chn_arr;
		std::atomic <bool>
		               _clip_flag;
	};

	               MeterResultSet ();
	virtual        ~MeterResultSet () = default;

	void           reset ();
	bool           check_signal_clipping ();

	std::array <Side, Dir_NBR_ELT>
	               _audio_io;

	MeterResult    _dsp_use; // In [0 ; 1]
	std::atomic <bool>
	               _dsp_overload_flag;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               MeterResultSet (const MeterResultSet &other)    = delete;
	MeterResultSet &
	               operator = (const MeterResultSet &other)        = delete;
	bool           operator == (const MeterResultSet &other) const = delete;
	bool           operator != (const MeterResultSet &other) const = delete;

}; // class MeterResultSet



}  // namespace mfx



//#include "mfx/MeterResultSet.hpp"



#endif   // mfx_MeterResultSet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
