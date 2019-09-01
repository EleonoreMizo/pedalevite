/*****************************************************************************

        SampleData.h
        Author: Laurent de Soras, 2019

The tables are octave-spaced. The provided table is always the lowest level,
numbered 0 (whereas 0 was the nyquist table for WavetableData). The original
table can be of any length.

It is up to the client to build the tables. It should use the loop point to
extrapolate data for high tables where the length is fractionnal. Same for
the table pre/post-unrolling, it has to be done by the client.

Actually this class is only a data storage. Every table should be initialized
before using it.

SDTP is the template parameter for sample data (for example SampleDataTplPar).
It should provide at least:

- typename SDTP::DataType, the type of data contained in the tables
- const int SDTP::NBR_TABLES, number of mip-maps, > 0.
- const int SDTP::UNROLL_PRE, number of extra-samples at the begining
	of each table, for interpolation/loop unrolling, >= 0.
- const int SDTP::UNROLL_POST, number of extra-samples at the end of
	each table, >= 0.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_SampleData_HEADER_INCLUDED)
#define mfx_dsp_osc_SampleData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace fstb
{
	class FixedPoint;
}

namespace mfx
{
namespace dsp
{
namespace osc
{



template <class SDTP>
class SampleData
{
	static_assert (SDTP::NBR_TABLES > 0, "");
	static_assert (SDTP::UNROLL_PRE >= 0, "");
	static_assert (SDTP::UNROLL_POST >= 0, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef SDTP ParamType;

	typedef typename SDTP::DataType DataType;

	static const int  NBR_TABLES  = ParamType::NBR_TABLES;
	static const int  UNROLL_PRE  = ParamType::UNROLL_PRE;
	static const int  UNROLL_POST = ParamType::UNROLL_POST;

	               SampleData ();
	               ~SampleData ()                       = default;
	               SampleData (const SampleData &other) = default;
	SampleData &   operator = (const SampleData &other) = default;

	bool           is_valid () const;

	void           set_main_info (int len);
	void           set_table_base_address (int table, DataType *data_ptr);
	inline void    set_sample (int table, int pos, DataType val);
	fstb_FORCEINLINE DataType
	               get_sample (int table, int pos) const;
	inline DataType *
	               use_table (int table);
	inline const DataType *
	               use_table (int table) const;

	inline int     get_nbr_tables () const;
	inline int     get_table_len (int table) const;
	inline int     get_unrolled_table_len (int table) const;

	static fstb_FORCEINLINE void
	               convert_position (fstb::FixedPoint &table_pos, const fstb::FixedPoint &abs_pos, int table);
	static fstb_FORCEINLINE void
	               convert_position (fstb::FixedPoint &pos, int table);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	typedef std::array <DataType *, NBR_TABLES> TablePtrArray;
	typedef std::array <int, NBR_TABLES> TableLenArray;

	TablePtrArray  _table_ptr_arr;	// Stores the pointer on 1st non-unrolled data.
	TableLenArray  _table_len_arr;	// Rounded to +oo, without unrolling



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const SampleData &other) const = delete;
	bool           operator != (const SampleData &other) const = delete;

}; // class SampleData



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/SampleData.hpp"



#endif   // mfx_dsp_osc_SampleData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
