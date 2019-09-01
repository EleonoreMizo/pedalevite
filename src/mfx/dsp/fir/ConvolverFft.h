/*****************************************************************************

        ConvolverFft.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_ConvolverFft_HEADER_INCLUDED)
#define mfx_dsp_fir_ConvolverFft_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "ffft/FFTReal.h"

#include <memory>
#include <vector>



namespace mfx
{
namespace dsp
{
namespace fir
{



class ConvolverFft
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef float DataType;

	               ConvolverFft ()  = default;
	explicit       ConvolverFft (int nbr_spl, const DataType impulse_ptr []);
	virtual        ~ConvolverFft () = default;

	bool           is_valid () const;
	void           set_impulse (int nbr_spl, const DataType impulse_ptr []);

	int            get_block_len () const;
	DataType *     get_input_buffer () const;
	const DataType *
	               get_output_buffer () const;

	void           process ();

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef ffft::FFTReal <DataType> FftType;
	typedef std::shared_ptr <FftType> FftSPtr;
	typedef std::vector <DataType> Buffer;

	FftSPtr        _fft_sptr;
	Buffer         _impulse_freq;
	Buffer         _input_buf;
	Buffer         _temp_buf;			// For data in freq domain or 0-padded impulse in time domain
	Buffer         _output_buf;
	int            _impulse_len = 0; // > 0. 0 = not initialized
	int            _fft_len     = 0; // > _impulse_len. 0 = not initialized
	int            _block_len   = 0; // > 0. 0 = not initialized
	int            _rem_len     = 0; // > 0. 0 = not initialized



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ConvolverFft (const ConvolverFft &other)      = delete;
	ConvolverFft & operator = (const ConvolverFft &other)        = delete;
	bool           operator == (const ConvolverFft &other) const = delete;
	bool           operator != (const ConvolverFft &other) const = delete;

}; // class ConvolverFft



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/fir/ConvolverFft.hpp"



#endif   // mfx_dsp_fir_ConvolverFft_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
