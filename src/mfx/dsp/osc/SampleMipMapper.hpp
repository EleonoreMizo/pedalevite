/*****************************************************************************

        SampleMipMapper.hpp
        Author: Laurent de Soras, 2019

The half-band filter is generally not optimal because its size is
1+2^k = n*4+1 thus coefficients at the ends are 0. However the 1+2^k length
allows to control accurately the delay introduced by the filter on each layer,
in order every tables are "in-phase" with each other.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_SampleMipMapper_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_SampleMipMapper_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/fnc.h"
#include "mfx/dsp/wnd/CoefGenBHMinLobe.h"
#include "mfx/dsp/wnd/Generic.h"

#include <algorithm>

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class SD>
SampleMipMapper <SD>::SampleMipMapper ()
:	_data_ptr (nullptr)
,	_sample_provider_ptr (nullptr)
,	_buffer_arr ()
,	_buf_size (0)
{
	make_filter ();
}



template <class SD>
int	SampleMipMapper <SD>::build_mipmaps (SampleTable &data, SampleProviderInterface &sample_provider)
{
	assert (_buf_size > 0);
	_data_ptr            = &data;
	_sample_provider_ptr = &sample_provider;

	const int      end_pos = prepare_buffers ();
	int            ret_val =
		_sample_provider_ptr->restart (_buffer_arr [0]._buf_pos);

	// Loop on the data source blocks
	while (   ret_val == 0
	       && _buffer_arr [0]._buf_pos < end_pos)
	{
		// Get new samples for the first table
		assert (_buffer_arr [0]._fill_pos == 0);
		CalcDataType *		buf_ptr = _buffer_arr [0]._filter.get_input_buffer ();
		ret_val = sample_provider.get_block (buf_ptr, _buf_size);
		if (ret_val == 0)
		{
			_buffer_arr [0]._fill_pos = _buf_size;

			// If end is reached, force the downsampling and flushing of all tables
			const bool     finish_flag =
				((_buffer_arr [0]._buf_pos + _buffer_arr [0]._fill_pos) >= end_pos);

			// Build as many tables as we can, depending on the buffer
			// filling states.
			bool           cont_flag = true;
			int            table;
			for (table = 0;
			     table < SampleTable::NBR_TABLES - 1 && cont_flag;
			     ++table)
			{
				Layer &        cur_layer = _buffer_arr [table];

				// Enough samples are ready, downsample into the next table
				if (cur_layer._fill_pos >= _buf_size || finish_flag)
				{
					// Store data into table
					flush_buffer (table);

					// Filter
					cur_layer._filter.process ();

					// Decimate
					Layer &        next_layer    = _buffer_arr [table + 1];
					const int      half_buf_size = _buf_size >> 1;
					const CalcDataType * src_ptr =
						cur_layer._filter.get_output_buffer ();
					assert (   next_layer._fill_pos == 0
					        || next_layer._fill_pos == half_buf_size);
					CalcDataType * dest_ptr =
						next_layer._filter.get_input_buffer () + next_layer._fill_pos;
					for (int pos = 0; pos < half_buf_size; ++pos)
					{
						dest_ptr [pos] = src_ptr [pos * 2 + 1];
					}
					next_layer._fill_pos += half_buf_size;
				}

				// Not enough samples
				else
				{
					cont_flag = false;
				}
			}

			// Store data into table if required
			check_and_flush_buffer (table, finish_flag);
		}
	}
	
	_sample_provider_ptr->stop ();
	_sample_provider_ptr = nullptr;
	_data_ptr            = nullptr;

	return ret_val;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class SD>
void	SampleMipMapper <SD>::make_filter ()
{
	Buffer         filter_impulse (_filter_size, 0);

	// Filter impulse: sinc
	{
		const int      center = _half_filter_size;
		for (int pos = 1; pos < _half_filter_size; pos += 2)
		{
			const CalcDataType	val = CalcDataType (fstb::sinc (pos * 0.5));
			filter_impulse [center - pos] = val;
			filter_impulse [center + pos] = val;
		}
		filter_impulse [center] = 1;
	}

	// Window
	{
		wnd::Generic <CalcDataType, wnd::CoefGenBHMinLobe>  window_maker;
		Buffer            window_data (_filter_size);
		window_maker.make_win (&window_data [0], _filter_size);
		const CalcDataType   scale = 0.5f;  // = 1 / downsampling ratio
		for (int pos = 0; pos < _filter_size; ++pos)
		{
			filter_impulse [pos] *= window_data [pos] * scale;
		}
	}

	// Configure all convolvers
	for (int table = 0; table < SampleTable::NBR_TABLES; ++table)
	{
		_buffer_arr [table]._filter.set_impulse (
			_filter_size,
			&filter_impulse [0]
		);
	}

	_buf_size = _buffer_arr [0]._filter.get_block_len ();
	assert ((_buf_size & 1) == 0);
}



template <class SD>
int	SampleMipMapper <SD>::prepare_buffers ()
{
	assert (_buf_size > 0);
	assert (_data_ptr != nullptr);
	assert (_sample_provider_ptr != nullptr);

	// Request enough samples to build the pre-unrolling data for
	// the highest table. Initialize buffer data.
	const int      pre_roll =
		SampleTable::UNROLL_PRE << (SampleTable::NBR_TABLES - 1);

	// Same with the delay introduced by the filter
	const int      filter_delay = _half_filter_size;
	const int      pre_delay    =
		filter_delay << (SampleTable::NBR_TABLES - 1);

	for (int buf = 0; buf < SampleTable::NBR_TABLES; ++buf)
	{
		_buffer_arr [buf]._buf_pos = -((pre_roll + pre_delay) >> buf);

		// I'm not very sure about this one. It is based on the fact that
		// the delay introduced by the filter for a particular table can be
		// expressed as :
		// d(n) = d(n-1)/2 + filter_delay
		// where n is the table number, and d(0) = 0. Therefore :
		// d(n) = filter_delay * (1 - 2 / 2^n)
		_buffer_arr [buf]._buf_pos -= filter_delay - (filter_delay >> buf);

		_buffer_arr [buf]._fill_pos = 0;
		_buffer_arr [buf]._filter.clear_buffers ();
		_buffer_arr [buf]._beg_pos  = -SampleTable::UNROLL_PRE;
		_buffer_arr [buf]._end_pos  =
			_data_ptr->get_table_len (buf) + SampleTable::UNROLL_POST;
	}

	// We should take into account the delay introduced by the filter on
	// the latest table, which leads to filter_delay. This is an upper bound
	// so we don't care if it is not exactly the value.
	const int      end_pos_last_table =
		_buffer_arr [SampleTable::NBR_TABLES - 1]._end_pos + filter_delay;
	const int      end_pos =
		end_pos_last_table << (SampleTable::NBR_TABLES - 1);

	return end_pos;
}



template <class SD>
void	SampleMipMapper <SD>::check_and_flush_buffer (int table, bool force_flag)
{
	assert (_buf_size > 0);
	assert (_data_ptr != nullptr);
	assert (_sample_provider_ptr != nullptr);

	assert (table >= 0);
	assert (table < SampleTable::NBR_TABLES);

	const Layer &  layer = _buffer_arr [table];
	if (layer._fill_pos >= _buf_size || force_flag)
	{
		flush_buffer (table);
	}
}



template <class SD>
void	SampleMipMapper <SD>::flush_buffer (int table)
{
	assert (_buf_size > 0);
	assert (_data_ptr != nullptr);
	assert (_sample_provider_ptr != nullptr);

	assert (table >= 0);
	assert (table < SampleTable::NBR_TABLES);

	Layer &        layer = _buffer_arr [table];
	if (layer._fill_pos > 0)
	{
		store_data_into_tables (table);
	}

	layer._buf_pos += layer._fill_pos;
	layer._fill_pos = 0;
}



template <class SD>
void	SampleMipMapper <SD>::store_data_into_tables (int table)
{
	assert (_buf_size > 0);
	assert (_data_ptr != nullptr);
	assert (_sample_provider_ptr != nullptr);

	assert (table >= 0);
	assert (table < SampleTable::NBR_TABLES);
	Layer &        layer = _buffer_arr [table];

	const int      zone_start = layer._buf_pos;
	const int      real_start = std::max (int (zone_start), layer._beg_pos);
	const int      zone_end   = layer._buf_pos + layer._fill_pos;
	const int      real_end   = std::min (zone_end, layer._end_pos);
	const int      nbr_spl    = real_end - real_start;
	if (nbr_spl > 0)
	{
		const int      skip = real_start - zone_start;
		StorageDataType * dest_ptr = _data_ptr->use_table (table) + real_start;
		const CalcDataType * src_ptr = layer._filter.get_input_buffer () + skip;

		_sample_provider_ptr->store_block (dest_ptr, src_ptr, nbr_spl);
	}
}



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_SampleMipMapper_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
