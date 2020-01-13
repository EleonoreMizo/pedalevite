/*****************************************************************************

        SampleMipMapper.h
        Author: Laurent de Soras, 2019

Client application must implement an interface to retrieve sample data
linearly. If SampleMipMapper asks for more data than it is really needed,
the interface should provide with looped samples or zeros. The restart
position is generally negative to satisfy the unrolling terms.

The SampleData object should be already allocated and initialized, but not
filled at all.

Implementation:

All the mip-maps are built in parallel. An array of buffers stores input data
for each mip-map. Once a buffer is full, it is stored in the mipmap, then
filtered and decimated to feed the next buffer.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_SampleMipMapper_HEADER_INCLUDED)
#define mfx_dsp_osc_SampleMipMapper_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/fir/ConvolverFft.h"

#include <array>
#include	<vector>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <class SD>
class SampleMipMapper
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef SampleMipMapper <SD> ThisType;
	typedef SD SampleTable;
	typedef typename SD::DataType StorageDataType;
	typedef typename fir::ConvolverFft::DataType CalcDataType;

	class SampleProviderInterface
	{
	public:
		typedef ThisType MipMapperType;
		               SampleProviderInterface ()  = default;
		               SampleProviderInterface (const SampleProviderInterface &other) = default;
		               SampleProviderInterface (SampleProviderInterface &&other)      = default;
		virtual        ~SampleProviderInterface () = default;
		SampleProviderInterface &
		               operator = (const SampleProviderInterface &other) = default;
		SampleProviderInterface &
		               operator = (SampleProviderInterface &&other)      = default;
		virtual int    restart (int offset) = 0;
		virtual int    get_block (CalcDataType data_ptr [], int nbr_spl) = 0;
		virtual void   store_block (StorageDataType dest_ptr [], const CalcDataType src_ptr [], int nbr_spl) = 0;
		virtual void   stop () = 0;
	};

	               SampleMipMapper ();
	virtual        ~SampleMipMapper () = default;

	int            build_mipmaps (SampleTable &data, SampleProviderInterface &sample_provider);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::vector <CalcDataType> Buffer;

	class Layer
	{
	public:
		fir::ConvolverFft
		               _filter;
		int            _buf_pos  { -1 }; // Start of the buffer relatively to the beginning of the table
		int            _fill_pos { -1 }; // Filling progress indicator
		int            _beg_pos  { -1 }; // Start of the buffer, with pre-unrolling
		int            _end_pos  { -1 }; // End of the buffer, with post-unrolling
	};

	typedef std::array <Layer, SampleTable::NBR_TABLES> BufferArray;

	void           make_filter ();
	int            prepare_buffers ();
	void           check_and_flush_buffer (int table, bool force_flag);
	void           flush_buffer (int table);
	void           store_data_into_tables (int table);

	SampleTable *  _data_ptr;     // 0 = not initialized.
	SampleProviderInterface *     // 0 = not initialized.
	               _sample_provider_ptr;
	BufferArray    _buffer_arr;   // Temporary buffers for mip-mapping.
	int            _buf_size;     // Should be even. 0 = not initialized.

	static const int              // Ideally a power of 2, but it's not mandatory
	               _half_filter_size = 64;
	static const int              // Always odd, based on _half_filter_size.
	               _filter_size = _half_filter_size * 2 + 1;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SampleMipMapper (const SampleMipMapper &other)   = delete;
	               SampleMipMapper (SampleMipMapper &&other)        = delete;
	SampleMipMapper &
	               operator = (const SampleMipMapper &other)        = delete;
	SampleMipMapper &
	               operator = (SampleMipMapper &&other)             = delete;
	bool           operator == (const SampleMipMapper &other) const = delete;
	bool           operator != (const SampleMipMapper &other) const = delete;

}; // class SampleMipMapper



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/SampleMipMapper.hpp"



#endif   // mfx_dsp_osc_SampleMipMapper_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
