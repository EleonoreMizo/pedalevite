/*****************************************************************************

        SpeakerEmuDesc.h
        Copyright (c) 2016 Ohm Force

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_spkem_SpeakerEmuDesc_HEADER_INCLUDED)
#define mfx_pi_spkem_SpeakerEmuDesc_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/pi/ParamDescSet.h"
#include "mfx/piapi/PluginDescInterface.h"



namespace mfx
{
namespace pi
{
namespace spkem
{



class SpeakerEmuDesc
:	public piapi::PluginDescInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       SpeakerEmuDesc ();
	virtual        ~SpeakerEmuDesc () = default;

	ParamDescSet & use_desc_set ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// mfx::piapi::PluginDescInterface
	virtual std::string
	               do_get_unique_id () const;
	virtual std::string
	               do_get_name () const;
	virtual void   do_get_nbr_io (int &nbr_i, int &nbr_o, int &nbr_s) const;
	virtual bool   do_prefer_stereo () const;
	virtual int    do_get_nbr_param (piapi::ParamCateg categ) const;
	virtual const piapi::ParamDescInterface &
	               do_get_param_info (piapi::ParamCateg categ, int index) const;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	ParamDescSet   _desc_set;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               SpeakerEmuDesc (const SpeakerEmuDesc &other)    = delete;
	SpeakerEmuDesc &
	               operator = (const SpeakerEmuDesc &other)        = delete;
	bool           operator == (const SpeakerEmuDesc &other) const = delete;
	bool           operator != (const SpeakerEmuDesc &other) const = delete;

}; // class SpeakerEmuDesc



}  // namespace spkem
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/spkem/SpeakerEmuDesc.hpp"



#endif   // mfx_pi_spkem_SpeakerEmuDesc_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
