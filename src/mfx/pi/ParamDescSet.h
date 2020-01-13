/*****************************************************************************

        ParamDescSet.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ParamDescSet_HEADER_INCLUDED)
#define mfx_pi_ParamDescSet_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/piapi/ParamCateg.h"
#include "mfx/piapi/ParamDescInterface.h"

#include <array>
#include <memory>
#include <vector>



namespace mfx
{
namespace pi
{



class ParamDescSet
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ParamDescSet (int nbr_glob, int nbr_notes);

	template <typename T>
	void           add (piapi::ParamCateg categ, int index, std::shared_ptr <T> &sptr);
	template <typename T>
	void           add_glob (int index, std::shared_ptr <T> &sptr);
	template <typename T>
	void           add_note (int index, std::shared_ptr <T> &sptr);

	bool           is_ready () const;

	int            get_nbr_param (piapi::ParamCateg categ) const;
	const piapi::ParamDescInterface &
	               use_param (piapi::ParamCateg categ, int index) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef std::shared_ptr <piapi::ParamDescInterface> ParamSPtr;
	typedef std::vector <ParamSPtr> ParamArray;
	typedef std::array <ParamArray, piapi::ParamCateg_NBR_ELT> CategArray;

	CategArray     _categ_arr;
	int            _nbr_rem_param;      // Mostly for debugging



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ParamDescSet ()                               = delete;
	               ParamDescSet (const ParamDescSet &&other)     = delete;
	               ParamDescSet (const ParamDescSet &other)      = delete;
	ParamDescSet & operator = (const ParamDescSet &other)        = delete;
	ParamDescSet & operator = (const ParamDescSet &&other)       = delete;
	bool           operator == (const ParamDescSet &other) const = delete;
	bool           operator != (const ParamDescSet &other) const = delete;

}; // class ParamDescSet



}  // namespace pi
}  // namespace mfx



#include "mfx/pi/ParamDescSet.hpp"



#endif   // mfx_pi_ParamDescSet_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
