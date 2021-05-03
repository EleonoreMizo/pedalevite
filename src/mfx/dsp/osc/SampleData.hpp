/*****************************************************************************

        SampleData.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_SampleData_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_SampleData_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/FixedPoint.h"

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <typename SDTP>
SampleData <SDTP>::SampleData () noexcept
:	_table_ptr_arr ()
,	_table_len_arr ()
{
	_table_ptr_arr.fill (nullptr);
	_table_len_arr.fill (0);
}



template <typename SDTP>
bool	SampleData <SDTP>::is_valid () const noexcept
{
	bool           valid_flag = true;
	for (int table = 0; table < NBR_TABLES && valid_flag; ++table)
	{
		valid_flag = (   _table_len_arr [table] > 0
		              && _table_ptr_arr [table] != nullptr);
	}

	return valid_flag;
}



template <typename SDTP>
void	SampleData <SDTP>::set_main_info (int len) noexcept
{
	assert (len > 0);

	for (int table = 0; table < NBR_TABLES; ++table)
	{
		_table_len_arr [table] = len;
		len = (len + 1) >> 1;
	}
}



/*
==============================================================================
Name: set_table_base_address
Description:
	Provide table data. This is not exactly the opposite of use_table(),
	because use_table() returns a pointer on the first NON-unrolled sample.
Input parameters:
	- table: Table index. 0 = biggest one.
	- data_ptr: Pointer on the table. It should point on the first unrolled
		data.
Throws: Nothing
==============================================================================
*/

template <typename SDTP>
void	SampleData <SDTP>::set_table_base_address (int table, DataType *data_ptr) noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);
	assert (data_ptr != nullptr);

	_table_ptr_arr [table] = data_ptr + UNROLL_PRE;
}



template <typename SDTP>
void	SampleData <SDTP>::set_sample (int table, int pos, DataType val) noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);
	assert (pos >= -UNROLL_PRE);
	assert (pos < get_table_len (table) + UNROLL_POST);

	use_table (table) [pos] = val;
}



template <typename SDTP>
typename SampleData <SDTP>::DataType	SampleData <SDTP>::get_sample (int table, int pos) const noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);
	assert (pos >= -UNROLL_PRE);
	assert (pos < get_table_len (table) + UNROLL_POST);

	return use_table (table) [pos];
}



/*
==============================================================================
Name : use_table
Description:
	Get the pointer on the begining of the table ; the first NON-unrolled
	sample data. Therefore the pointer can be used directly to index samples in
	the table.
Input parameters:
	- table: Table index, [0 ; N-1]
Returns: Pointer on the table
Throws: Nothing
==============================================================================
*/

template <typename SDTP>
typename SampleData <SDTP>::DataType *	SampleData <SDTP>::use_table (int table) noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);
	assert (_table_ptr_arr [table] != nullptr);

	return _table_ptr_arr [table];
}



template <typename SDTP>
const typename SampleData <SDTP>::DataType *	SampleData <SDTP>::use_table (int table) const noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);
	assert (_table_ptr_arr [table] != nullptr);

	return _table_ptr_arr [table];
}



template <typename SDTP>
int	SampleData <SDTP>::get_nbr_tables () const noexcept
{
	return NBR_TABLES;
}



/*
==============================================================================
Name: get_table_len
Description:
	Returns the table length in samples, without taking into account the
	unrolled data.
Input parameters:
	- table: Table index, [0 ; N-1]
Returns: Table length in samples
Throws: Nothing
==============================================================================
*/

template <typename SDTP>
int	SampleData <SDTP>::get_table_len (int table) const noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);

	return _table_len_arr [table];
}



template <typename SDTP>
int	SampleData <SDTP>::get_unrolled_table_len (int table) const noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);

	return get_table_len (table) + UNROLL_PRE + UNROLL_POST;
}



template <typename SDTP>
void	SampleData <SDTP>::convert_position (fstb::FixedPoint &table_pos, const fstb::FixedPoint &abs_pos, int table) noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);

	table_pos = abs_pos;
	convert_position (table_pos, table);
}



template <typename SDTP>
void	SampleData <SDTP>::convert_position (fstb::FixedPoint &pos, int table) noexcept
{
	assert (table >= 0);
	assert (table < NBR_TABLES);

	pos >>= table;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_SampleData_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
