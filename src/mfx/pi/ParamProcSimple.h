/*****************************************************************************

        ParamProcSimple.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_ParamProcSimple_HEADER_INCLUDED)
#define mfx_pi_ParamProcSimple_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{

namespace piapi
{
	class ProcInfo;
}

namespace pi
{



class ParamStateSet;

class ParamProcSimple
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       ParamProcSimple (ParamStateSet &state_set);
	virtual        ~ParamProcSimple () = default;

	void           handle_msg (piapi::ProcInfo &proc);
	bool           is_req_steady_state () const;
	bool           is_full_reset () const;

	void           req_steady ();
	void           req_all ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ParamStateSet& _state_set;
	bool           _steady_req_flag;
	bool           _full_reset_flag;

	bool           _nxt_steady_req_flag;
	bool           _nxt_full_reset_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ParamProcSimple ()                               = delete;
	               ParamProcSimple (const ParamProcSimple &other)   = delete;
	ParamProcSimple &
	               operator = (const ParamProcSimple &other)        = delete;
	bool           operator == (const ParamProcSimple &other) const = delete;
	bool           operator != (const ParamProcSimple &other) const = delete;

}; // class ParamProcSimple



}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/ParamProcSimple.hpp"



#endif   // mfx_pi_ParamProcSimple_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
