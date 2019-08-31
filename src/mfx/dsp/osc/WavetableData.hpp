/*****************************************************************************

        WavetableData.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_dsp_osc_WavetableData_CODEHEADER_INCLUDED)
#define mfx_dsp_osc_WavetableData_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace dsp
{
namespace osc
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::WavetableData ()
:	_data ()
{
	// Build tables. Not a big deal if it's done more than once...
	build_table_index ();
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::set_sample (int table, int pos, DataType val)
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);
	assert (pos >= 0);
	assert (pos < get_table_len (table));

	_data [_table_index [table] + pos] = val;
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::DataType	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::get_sample (int table, int pos) const
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);
	assert (pos >= -UNROLL_PRE);
	assert (pos < get_table_len (table) + UNROLL_POST);

	return _data [_table_index [table] + pos];
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::DataType *	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::use_table (int table)
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);

	return &_data [_table_index [table]];
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
const typename WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::DataType *	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::use_table (int table) const
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);

	return &_data [_table_index [table]];
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::update_table_unroll (int table)
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);

	const int      start = _table_index [table];
	const int      end   = start + get_table_len (table);

	// Pre-roll
	for (int pos = 1; pos <= UNROLL_PRE; ++pos)
	{
		_data [start - pos] = _data [end - pos];
	}

	// Post-roll
	for (int pos = 0; pos < UNROLL_POST; ++pos)
	{
		_data [end + pos] = _data [start + pos];
	}
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::update_all_tables_unroll ()
{
	const int      nbr_tables = get_nbr_tables ();
	for (int table = 0; table < nbr_tables; ++table)
	{
		update_table_unroll (table);
	}
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::clear_table (int table)
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);

	const int      table_len = get_table_len (table);
	for (int pos = 0; pos < table_len; ++pos)
	{
		set_sample (table, pos, DataType (0));
	}
	update_table_unroll (table);
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
int	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::get_nbr_tables ()
{
	return MAX_SIZE_LOG2 + 1;
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
int	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::get_table_len (int table)
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);

	return _table_size [table];
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
int	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::get_table_len_log2 (int table)
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);

	return _table_size_log2 [table];
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
int	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::get_table_mask (int table)
{
	assert (table >= 0);
	assert (table <= MAX_SIZE_LOG2);

	return get_table_len (table) - 1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
void	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::build_table_index ()
{
	int            index      = DATA_ARR_SIZE;

	const int      nbr_tables = get_nbr_tables ();
	for (int table = 0; table < nbr_tables; ++table)
	{
		const int      table_size_l2 =
			(table < MIN_SIZE_LOG2) ? MIN_SIZE_LOG2 : table;
		const int      table_size = 1L << table_size_l2;

		_table_size_log2 [table] = table_size_l2;
		_table_size [table]      = table_size;
		index                   -= table_size + UNROLL_POST;
		_table_index [table]     = index;
		index                   -= UNROLL_PRE;
	}

	assert (index == 0);
}



template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::TableInfArray	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::_table_index;

template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::TableInfArray	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::_table_size;

template <int MAXSL2, int MINSL2, typename DT, int UPRE, int UPOST>
typename WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::TableInfArray	WavetableData <MAXSL2, MINSL2, DT, UPRE, UPOST>::_table_size_log2;



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#endif   // mfx_dsp_osc_WavetableData_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
