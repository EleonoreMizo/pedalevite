/*****************************************************************************

        TestSampleMipMapper.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (TestSampleMipMapper_HEADER_INCLUDED)
#define TestSampleMipMapper_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/osc/SampleData.h"
#include "mfx/dsp/osc/SampleDataTplPar.h"
#include "mfx/dsp/osc/SampleMipMapper.h"

#include <vector>

#include <cstdint>



class TestSampleMipMapper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef int16_t StorageType;
	typedef mfx::dsp::osc::SampleDataTplPar <8, StorageType, 1, 2> TplPar;
	typedef mfx::dsp::osc::SampleData <TplPar> SampleDataType;

	explicit       TestSampleMipMapper (int test_len);
	virtual        ~TestSampleMipMapper () = default;

	SampleDataType	&
	               use_sample_data ();

	static int     perform_test ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef mfx::dsp::osc::SampleMipMapper <SampleDataType> MipMapper;
	typedef std::vector <StorageType> Buffer;

	class Provider
	:	public MipMapper::SampleProviderInterface
	{
	public:
		               Provider (int size);

		// MipMapper::SampleProviderInterface
		virtual int    restart (int offset);
		virtual int    get_block (MipMapper::CalcDataType data_ptr [], int nbr_spl);
		virtual void   store_block (MipMapper::StorageDataType dest_ptr [], const MipMapper::CalcDataType src_ptr [], int nbr_spl);
		virtual void   stop ();
	private:
		int            _size;
		int            _pos;
	};

	std::vector <Buffer>
	               _table_data_arr;
	SampleDataType _sd;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               TestSampleMipMapper ()                               = delete;
	               TestSampleMipMapper (const TestSampleMipMapper &other) = delete;
	TestSampleMipMapper &
	               operator = (const TestSampleMipMapper &other)        = delete;
	bool           operator == (const TestSampleMipMapper &other) const = delete;
	bool           operator != (const TestSampleMipMapper &other) const = delete;

}; // class TestSampleMipMapper



//#include "TestSampleMipMapper.hpp"



#endif   // TestSampleMipMapper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
