/*****************************************************************************

        SlidingMax.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_fir_SlidingMax_CODEHEADER_INCLUDED)
#define mfx_dsp_fir_SlidingMax_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace fir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: set_length
Description:
	Sets the size of the window. Past samples are lost.
Input parameters:
	- len: size of the window, > 0.
Throws: depends on std::vector
==============================================================================
*/

template <typename T>
void	SlidingMax <T>::set_length (int len)
{
	assert (len > 0);

	_len      = len;
	_flip_beg = ((_len - 1) >> 1) ^ (_len - 1);
	_flip_end =  (_len + 1) >> 1;

	_data.resize (_len);
	clear_buffers ();
}



/*
==============================================================================
Name: clear_buffers
Description:
	Erase all past samples.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	SlidingMax <T>::clear_buffers () noexcept
{
	fill (std::numeric_limits <DataType>::lowest ());
}



/*
==============================================================================
Name: fill
Description:
	Fills the internal state with a given value, like if the past input samples
	were all equal to this value.
Input parameters:
	- val: initialisation value
Throws: Nothing
==============================================================================
*/

template <typename T>
void	SlidingMax <T>::fill (const DataType &val) noexcept
{
	_inmax    = val;
	_scanmax  = _inmax;
	std::fill (_data.begin (), _data.end (), _inmax);
	_writepos = 0;
	_scan_pos = 0;
	_scan_end = 0;
	_scan_beg = (_len - 1) >> 1;
}



/*
==============================================================================
Name: process_sample
Description:
	Inserts a new sample, shifts the window and computes the max.
	If there is not enough past samples to fill a full window, the window is
	shortened to the available samples.
Input parameters:
	- x: The new sample
Returns: the result of the operation on all the operands from the new window.
Throws: Nothing
==============================================================================
*/

template <typename T>
typename SlidingMax <T>::DataType	SlidingMax <T>::process_sample (DataType x) noexcept
{
	if (_len == 1)
	{
		return x;
	}

	--_scan_pos;
	if (_scan_pos >= _scan_end)
	{
		_inmax            = std::max (_inmax, x);
		_data [_scan_pos] = std::max (_data [_scan_pos], _data [_scan_pos + 1]);
	}
	else
	{
		_scanmax   = _inmax;
		_inmax     = x;
		_scan_end  = _scan_end ^ _flip_end;
		_scan_beg  = _scan_beg ^ _flip_beg;
		_scan_pos  = _scan_beg;
	}
 
	_data [_writepos] = x;
	++ _writepos;
	if (_writepos >= _len)
	{
		_writepos = 0;
	}
	const DataType outmax    = _data [_writepos];
	const DataType movingmax = std::max (_inmax, std::max (_scanmax, outmax));

	return movingmax;
}



/*
==============================================================================
Name: process_block
Description:
	Inserts new samples and for each sample, shifts the window and computes the
	max.
	Can work in-place.
Input parameters:
	- src_ptr: Pointer on the input buffer. Not null.
	- nbr_spl: Number of samples to process. > 0.
Output parameters:
	- dst_ptr: Pointer on the preallocated result buffer. Not null.
Throws: Nothing
==============================================================================
*/

template <typename T>
void	SlidingMax <T>::process_block (DataType dst_ptr [], const DataType src_ptr [], int nbr_spl) noexcept
{
	assert (dst_ptr != nullptr);
	assert (src_ptr != nullptr);
	assert (nbr_spl > 0);

	for (int pos = 0; pos < nbr_spl; ++pos)
	{
		dst_ptr [pos] = process_sample (src_ptr [pos]);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_fir_SlidingMax_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
