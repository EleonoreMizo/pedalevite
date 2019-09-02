/*****************************************************************************

        WavetableMipMapper.h
        Author: Laurent de Soras, 2019

Template parameters:

- WT: wavetable storage type, for example a WavetableData.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_WavetableMipMapper_HEADER_INCLUDED)
#define mfx_dsp_osc_WavetableMipMapper_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/FFTRealFixLen.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <class WT>
class WavetableMipMapper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef WT WavetableDataType;

	               WavetableMipMapper ()  = default;
	virtual        ~WavetableMipMapper () = default;

	void           build_mipmaps (WavetableDataType &wavetable);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef ffft::FFTRealFixLen <WavetableDataType::MAX_SIZE_LOG2> FftType;
	typedef std::shared_ptr <FftType> FftSPtr;
	typedef std::vector <typename FftType::DataType> TmpBuffer;

	FftSPtr        _fft_sptr = FftSPtr (new FftType);
	TmpBuffer      _bins     = TmpBuffer (WavetableDataType::MAX_SIZE);
	TmpBuffer      _data     = TmpBuffer (WavetableDataType::MAX_SIZE);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               WavetableMipMapper (const WavetableMipMapper &other) = delete;
	WavetableMipMapper &
	               operator = (const WavetableMipMapper &other)        = delete;
	bool           operator == (const WavetableMipMapper &other) const = delete;
	bool           operator != (const WavetableMipMapper &other) const = delete;

}; // class WavetableMipMapper



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/WavetableMipMapper.hpp"



#endif   // mfx_dsp_osc_WavetableMipMapper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
