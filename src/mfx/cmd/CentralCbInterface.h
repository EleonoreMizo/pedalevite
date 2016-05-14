/*****************************************************************************

        CentralCbInterface.h
        Copyright (c) 2016 Ohm Force

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_CentralCbInterface_HEADER_INCLUDED)
#define mfx_cmd_CentralCbInterface_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{

class Msg;

namespace cmd
{



class CentralCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	virtual        ~CentralCbInterface () = default;

	void           process_msg_audio_to_cmd (const Msg &msg);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	virtual void   do_process_msg_audio_to_cmd (const Msg &msg) = 0;



}; // class CentralCbInterface



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/CentralCbInterface.hpp"



#endif   // mfx_cmd_CentralCbInterface_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
