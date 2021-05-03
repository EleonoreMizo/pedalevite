/*****************************************************************************

        RankSelA.h
        Author: Laurent de Soras, 2019

Selects the Nth-rank element from a running N-element list.
Vector version.
The class is optimised to select multiple and random ranks per processed
sample.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_RankSelA_HEADER_INCLUDED)
#define mfx_dsp_fir_RankSelA_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <vector>



namespace mfx
{
namespace dsp
{
namespace fir
{



class RankSelA
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	void           reserve (int len);
	void           set_len (int len, float x = 0);
	void           set_rank (int rank) noexcept;
	void           fill (float x) noexcept;

	float          process_sample (float x) noexcept;
	float          get_nth (int rank) const noexcept;
	void           process_block (float dst_ptr [], const float src_ptr [], int nbr_spl) noexcept;

	void           clear_buffers () noexcept;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class Element
	{
	public:
		float          _val;
		int            _index; // Between each operation, stored indexes are always < _index_cur
	};
	typedef std::vector <Element> ElementArray;

	void           insert_new_remove_old (float x) noexcept;
	void           check_index () noexcept;

	// The list is always ordered (by increasing _val)
	ElementArray   _elt_arr   = ElementArray (1, Element ({ 0.f, -1 }));
	int            _index_cur = 0;
	int            _rank      = 0;      // Nth elt to pick. [0 ; N-1]
	


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool           operator == (const RankSelA &other) const = delete;
	bool           operator != (const RankSelA &other) const = delete;

}; // class RankSelA



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/fir/RankSelA.hpp"



#endif   // mfx_dsp_fir_RankSelA_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
