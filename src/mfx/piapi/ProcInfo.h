/*****************************************************************************

        ProcInfo.h
        Author: Laurent de Soras, 2019

Rules for the buffers:
- Input and output buffers are always separate (no "in-place" processing)
- Different input channels can refer to the same buffer
- Different output channels can refer to the same buffer too, but in this
case the content is undefined and the result cannot be kept by the host
("trash" buffer)

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_piapi_ProcInfo_HEADER_INCLUDED)
#define mfx_piapi_ProcInfo_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/BypassState.h"
#include "mfx/piapi/Dir.h"

#include <array>



namespace mfx
{
namespace piapi
{



class EventTs;

class ProcInfo
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	class DirInfo
	{
	public:
		int            _nbr_chn  =  1;
		int            _nbr_pins = -1; // Negative = all pins can be assumed connected
	};
	typedef std::array <DirInfo, Dir_NBR_ELT> DirInfoArray;

	float * const *
	               _dst_arr   = nullptr;
	float * const *
	               _byp_arr   = nullptr;
	const float * const *
	               _src_arr   = nullptr;
	float * const *
	               _sig_arr   = nullptr;

	DirInfoArray   _dir_arr;
	BypassState    _byp_state = BypassState_IGNORE; // On input and output
	int            _nbr_spl   = 0;
	const EventTs * const *
	               _evt_arr   = nullptr;
	int            _nbr_evt   = 0;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const ProcInfo &other) const = delete;
	bool           operator != (const ProcInfo &other) const = delete;

}; // class ProcInfo



}  // namespace piapi
}  // namespace mfx



//#include "mfx/piapi/ProcInfo.hpp"



#endif   // mfx_piapi_ProcInfo_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
