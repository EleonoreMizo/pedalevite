/*****************************************************************************

        RankSelL.h
        Author: Laurent de Soras, 2019

Selects the Nth-rank element from a running N-element list.
Linked list version.
This class is optimised to select a single rank per processed sample.

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_dsp_fir_RankSelL_HEADER_INCLUDED)
#define mfx_dsp_fir_RankSelL_HEADER_INCLUDED

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



class RankSelL
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               RankSelL ()  = default;
	virtual        ~RankSelL () = default;

	void           reserve (int len);
	void           set_len (int len, float x = 0);
	void           set_rank (int rank);
	void           fill (float x);

	float          process_sample (float x);
	float          get_nth (int rank) const;
	void           process_block (float dst_ptr [], float src_ptr [], int nbr_spl);

	void           clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _nil = -1;

	class Node
	{
	public:
		int            _next;   // Not connected: _nil
		int            _prev;   // Not connected: _nil
		float          _val;
	};
	typedef std::vector <Node> NodeList;

	void           insert_new_remove_old (float x);
	int            find_ni (int rank);
	void           check_ok ();

	NodeList       _node_list = NodeList (1, Node ({ _nil, _nil, 0.f })); // Stored by insertion time (ring buffer), linked by increasing value
	int            _ni_first  = 0;      // Node index, first from the list
	int            _ni_last   = 0;      // Node index, last from the list
	int            _ni_rank   = 0;      // Direct access to the Nth element in _node_index
	int            _ni_w      = 0;      // Node being deleted/inserted
	int            _rank      = 0;      // Nth elt to pick. [0 ; N-1]
	


/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               RankSelL (const RankSelL &other)          = delete;
	RankSelL &     operator = (const RankSelL &other)        = delete;
	bool           operator == (const RankSelL &other) const = delete;
	bool           operator != (const RankSelL &other) const = delete;

}; // class RankSelL



}  // namespace fir
}  // namespace dsp
}  // namespace mfx



//#include "mfx/dsp/fir/RankSelL.hpp"



#endif   // mfx_dsp_fir_RankSelL_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
