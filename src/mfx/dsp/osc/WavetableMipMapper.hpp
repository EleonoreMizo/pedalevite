/*****************************************************************************

        WavetableMipMapper.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_WavetableMipMapper_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_WavetableMipMapper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class WT>
void	WavetableMipMapper <WT>::build_mipmaps (WavetableDataType &wavetable)
{
	const int      biggest_table = wavetable.get_nbr_tables () - 1;

	// Copies first table into data part
	typename WavetableDataType::DataType * bwt_ptr =
		wavetable.use_table (biggest_table);
	for (int i = 0; i < WavetableDataType::MAX_SIZE; ++i)
	{
		_data [i] = static_cast <typename FftType::DataType> (bwt_ptr [i]);
	}

	// Gets spectrum data of the longest wavetable
	_fft_sptr->do_fft (&_bins [0], &_data [0]);
	_fft_sptr->rescale (&_bins [0]);
	int            nbr_used_bins = WavetableDataType::MAX_SIZE >> 1;
	const int      imag_index    = nbr_used_bins;

	// Builds all other wavetables
	for (int table = biggest_table - 1; table >= 0; --table)
	{
		// Reduces spectrum bandwidth by 1/2
		const int      old_nbr_used_bins = nbr_used_bins;
		nbr_used_bins >>= 1;
		for (int bin = nbr_used_bins + 1; bin < old_nbr_used_bins; ++bin)
		{
			_bins [bin] = 0;
			_bins [imag_index + bin] = 0;
		}
		_bins [old_nbr_used_bins]          = 0; // Kills real part of old nyquist freq
		_bins [imag_index + nbr_used_bins] = 0; // Kills imag part of new nyquist freq

		// Transforms
		_fft_sptr->do_ifft (&_bins [0], &_data [0]);

		// Decimates
		const int      table_len = wavetable.get_table_len (table);
		const int      step      = WavetableDataType::MAX_SIZE / table_len;
		for (int pos = 0; pos < table_len; ++pos)
		{
			const typename WavetableDataType::DataType   val =
				static_cast <typename WavetableDataType::DataType> (_data [pos * step]);
			wavetable.set_sample (table, pos, val);
		}
	}

	wavetable.update_all_tables_unroll ();
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_WavetableMipMapper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
