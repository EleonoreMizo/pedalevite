/*****************************************************************************

        WavetableData.h
        Author: Laurent de Soras, 2019

Tables may be oversampled; the oversampling is constrained by the minimum and
maximum sizes of a wavetable.

Template parameters:

- MAXSL2: log2 of the maximum size of the wavetable
- MINSL2: log2 of the minimum size of the wavetable, [0 ; MAXSL2]
- OVRL2: log2 of the data oversampling, [0 ; MINSL2] 
- DT: type of stored data
- UPRE: number of unrolled samples at the begining of each wave, >= 0
- UPOST: number of unrolled samples at the end of each wave, >= 0

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_osc_WavetableData_HEADER_INCLUDED)
#define mfx_dsp_osc_WavetableData_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/def.h"

#include <array>



namespace mfx
{
namespace dsp
{
namespace osc
{



template <int MAXSL2, int MINSL2, int OVRL2, typename DT = float, int UPRE = 1, int UPOST = 3>
class WavetableData
{
	static_assert (MAXSL2 >= 0, "");
	static_assert (MAXSL2 <= 16, "");
	static_assert (MINSL2 >= 0, "");
	static_assert (MINSL2 <= MAXSL2, "");
	static_assert (OVRL2 >= 0, "");
	static_assert (OVRL2 <= MAXSL2 - MINSL2, "");
	static_assert (UPRE >= 0, "");
	static_assert (UPOST >= 0, "");

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static const int  MAX_SIZE_LOG2 = MAXSL2;
	static const int  MAX_SIZE      = 1 << MAX_SIZE_LOG2;
	static const int  MIN_SIZE_LOG2 = MINSL2;
	static const int  MIN_SIZE      = 1 << MIN_SIZE_LOG2;
	static const int  OVRSPL_LOG2   = OVRL2;
	static const int  OVRSPL        = 1 << OVRSPL_LOG2;
	static const int  UNROLL_PRE    = UPRE;
	static const int  UNROLL_POST   = UPOST;

	typedef	DT	DataType;

	               WavetableData ();
	               ~WavetableData () = default;

	inline void    set_sample (int table, int pos, DataType val);
	fstb_FORCEINLINE DataType
	               get_sample (int table, int pos) const;
	inline DataType *
	               use_table (int table);
	inline const DataType *
	               use_table (int table) const;

	void           update_table_unroll (int table);
	void           update_all_tables_unroll ();
	void           clear_table (int table);

	static inline int
	               get_nbr_tables ();
	static inline int
	               get_table_len (int table);
	static inline int
	               get_table_len_log2 (int table);
	static inline int
	               get_table_mask (int table);
	static inline int
	               get_table_ovr_log2 (int table);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// Wavetables from MAX_SIZE_LOG2-OVRSPL_LOG2 to MAX_SIZE_LOG2 are
	// of constant size
	static const int  UPPER_WT_SIZE = (1 << MAX_SIZE_LOG2) * OVRSPL_LOG2;

	// Wavetables between MIN_SIZE_LOG2-OVRSPL_LOG2 and MAX_SIZE_LOG2-OVRSPL_LOG2 are
	// 2^k-sample big.
	// Total size of these tables  :
	// size = sum (2^k, k=a..b)
	//      = 2^a * sum (2^k, k=0..b-a)
	//      = 2^a * (2^(b-a+1) - 1)
	static const int  OVR_WT_SIZE =
		((2 << (MAX_SIZE_LOG2 - MIN_SIZE_LOG2)) - 1) << MIN_SIZE_LOG2;

	// Wavetables between 0 and MIN_SIZE_LOG2-OVRSPL_LOG2 are 2^MIN_SIZE_LOG2-sample big
	static const int  LOWER_WT_SIZE =
		(1 << MIN_SIZE_LOG2) * (MIN_SIZE_LOG2 - OVRSPL_LOG2);

	// Add UNROLL_PRE and UNROLL_POST samples for every tables
	static const int  UNROLL_WT_SIZE =
		(MAX_SIZE_LOG2 + 1) * (UNROLL_PRE + UNROLL_POST);

	// Total size of all tables
	static const int  DATA_ARR_SIZE	=
		UPPER_WT_SIZE + OVR_WT_SIZE + LOWER_WT_SIZE + UNROLL_WT_SIZE;

	typedef std::array <DataType, DATA_ARR_SIZE> TableData;
	typedef std::array <int, MAX_SIZE_LOG2 + 1> TableInfArray;

	static void    build_table_index ();

	TableData      _data;

	static TableInfArray                // Table index in the data array
	               _table_index;
	static TableInfArray                // Table sizes
	               _table_size;
	static TableInfArray                // Log base 2 of table sizes
	               _table_size_log2;
	static TableInfArray                // Log base 2 of table acutal oversampling
	               _table_ovr_log2;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               WavetableData (const WavetableData &other)     = delete;
	WavetableData &
	               operator = (const WavetableData &other)        = delete;
	bool           operator == (const WavetableData &other) const = delete;
	bool           operator != (const WavetableData &other) const = delete;

}; // class WavetableData



}  // namespace osc
}  // namespace dsp
}  // namespace mfx



#include "mfx/dsp/osc/WavetableData.hpp"



#endif   // mfx_dsp_osc_WavetableData_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
