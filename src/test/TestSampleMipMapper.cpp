/*****************************************************************************

        TestSampleMipMapper.cpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"
#include "fstb/fnc.h"
#include "test/TestSampleMipMapper.h"
#include "test/FileOp.h"

#include <cassert>



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TestSampleMipMapper::TestSampleMipMapper (int test_len)
:	_table_data_arr (SampleDataType::NBR_TABLES)
,	_sd ()
{
	assert (test_len > 0);

	// Init data
	_sd.set_main_info (test_len);
	for (int table = 0; table < SampleDataType::NBR_TABLES; ++table)
	{
		const int      table_len = _sd.get_unrolled_table_len (table);
		_table_data_arr [table].resize (table_len);
		_sd.set_table_base_address (table, &_table_data_arr [table] [0]);
	}

	// Build mipmaps
	Provider       provider (test_len);
	MipMapper      mipmap;
	mipmap.build_mipmaps (_sd, provider);
}



TestSampleMipMapper::SampleDataType	&	TestSampleMipMapper::use_sample_data ()
{
	return _sd;
}



int	TestSampleMipMapper::perform_test ()
{
	const int      length = 50000;
	TestSampleMipMapper  tsmm (length);

	// Save data
	std::vector <float>  result_m;
	SampleDataType &  sd = tsmm.use_sample_data ();
	for (int table = 0; table < SampleDataType::NBR_TABLES; ++table)
	{
		const int      table_len = sd.get_table_len (table);
		const int      table_end = table_len + SampleDataType::UNROLL_POST;
		for (int pos = -SampleDataType::UNROLL_PRE; pos < table_end; ++pos)
		{
			const SampleDataType::DataType	val = sd.get_sample (table, pos);
			const float    spl = val * (1.0f / 0x8000);
			result_m.push_back (spl);
		}
	}

	FileOp::save_wav ("results/samplemipmapper0.wav", result_m, 44100, 0.5f);

	return 0;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



TestSampleMipMapper::Provider::Provider (int size)
:	_size (size)
,	_pos (0)
{
	assert (size >= 0);
}



int	TestSampleMipMapper::Provider::restart (int offset)
{
	_pos = offset;

	return 0;
}



int	TestSampleMipMapper::Provider::get_block (MipMapper::CalcDataType data_ptr [], int nbr_spl)
{
	for (int spl = 0; spl < nbr_spl; ++spl)
	{
		if (_pos >= 0 && _pos < _size)
		{
			data_ptr [spl] = MipMapper::CalcDataType (((_pos & 0xFF) - 0x80) * 0x100);
//			data_ptr [spl] = fstb::round_int (sin (_pos * (2 * fstb::PI * 15000/44100)) * 0x7FFF);
		}
		else
		{
			data_ptr [spl] = 0;
		}
		++_pos;
	}

	return (0);
}



void	TestSampleMipMapper::Provider::store_block (MipMapper::StorageDataType dest_ptr [], const MipMapper::CalcDataType src_ptr [], int nbr_spl)
{
	for (int spl = 0; spl < nbr_spl; ++spl)
	{
		dest_ptr [spl] = static_cast <MipMapper::StorageDataType> (
			fstb::limit (
				src_ptr [spl],
				MipMapper::CalcDataType (-0x8000),
				MipMapper::CalcDataType ( 0x7FFF)
			)
		);
	}
}



void	TestSampleMipMapper::Provider::stop ()
{
	// Nothing
}



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
