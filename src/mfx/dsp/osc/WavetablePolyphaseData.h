/*****************************************************************************

        WavetablePolyphaseData.h
        Author: Laurent de Soras, 2019

Template parameters:

- PLEN: phase length (only one phase)

- NPL2: Log2 of the number of phases

- DT: data type

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_WavetablePolyphaseData_HEADER_INCLUDED)
#define mfx_dsp_osc_WavetablePolyphaseData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <int PLEN, int NPL2, typename DT = float>
class WavetablePolyphaseData
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  PHASE_LEN       = PLEN;
	static const int  NBR_PHASES_LOG2 = NPL2;
	static const int  NBR_PHASES      = 1L << NPL2;
	static const int  TABLE_SIZE      = NBR_PHASES * PHASE_LEN;
	static const int  FIRST_ELT_INDEX = 0;

	typedef DT DataType;
	typedef WavetablePolyphaseData <PLEN, NPL2, DT> ThisType;

	inline void		set_sample (int phase, int pos, DataType val) noexcept;
	inline void		set_sample (int phase_pos, DataType val) noexcept;

	fstb_FORCEINLINE DataType
	               get_sample (int phase, int pos) const noexcept;
	fstb_FORCEINLINE DataType
	               get_sample (int phase_pos) const noexcept;

	fstb_FORCEINLINE DataType *
	               use_table (int phase) noexcept;
	fstb_FORCEINLINE const DataType *
	               use_table (int phase) const noexcept;

	void           clear_table (int phase) noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::array <DataType, PHASE_LEN> Phase;
	typedef std::array <Phase, NBR_PHASES>   PhaseArray;

	PhaseArray     _data_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const WavetablePolyphaseData &other) const = delete;
	bool           operator != (const WavetablePolyphaseData &other) const = delete;

}; // class WavetablePolyphaseData



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/WavetablePolyphaseData.hpp"



#endif   // mfx_dsp_osc_WavetablePolyphaseData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
