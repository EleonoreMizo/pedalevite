/*****************************************************************************

        PlugWrap.h
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (piapi2vst_PlugWrap_HEADER_INCLUDED)
#define piapi2vst_PlugWrap_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define	WIN32_LEAN_AND_MEAN
#define	NOMINMAX

#include "conc/CellPool.h"
#include "conc/LockFreeCell.h"
#include "conc/LockFreeQueue.h"
#include "fstb/AllocAlign.h"
#include "mfx/piapi/PluginDescInterface.h"
#include "mfx/piapi/PluginInterface.h"
#include "vstsdk24/pluginterfaces/vst2.x/aeffect.h"
#include "vstsdk24/pluginterfaces/vst2.x/aeffectx.h"

#if defined (_MSC_VER)
#include	<crtdbg.h>
#include	<new.h>
#include	<windows.h>
#endif	// _MSC_VER

#include <memory>
#include <vector>



// Must be defined elsewhere
extern mfx::piapi::FactoryInterface & main_use_plugin_factory ();



namespace mfx
{
	namespace piapi
	{
		class FactoryInterface;
	}
}

namespace piapi2vst
{



class PlugWrap
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	explicit       PlugWrap (audioMasterCallback audio_master, mfx::piapi::FactoryInterface &factory);
	virtual        ~PlugWrap ();

	::AEffect &    use_aeffect () { return _vst; }



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static const int  _max_nbr_evt = 4096;
	static const int  _max_nbr_chn = 
		  (mfx::piapi::PluginInterface::_max_nbr_chn <= 2)
		? mfx::piapi::PluginInterface::_max_nbr_chn
		: 2;

	typedef conc::LockFreeQueue <mfx::piapi::EventTs> EventQueue;
	typedef conc::CellPool <mfx::piapi::EventTs> EventPool;
	typedef conc::LockFreeCell <mfx::piapi::EventTs> EventCell;

	class EventLt
	{
	public:
		inline bool    operator () (mfx::piapi::EventTs * &lhs, mfx::piapi::EventTs * &rhs);
	};

	typedef std::vector <float, fstb::AllocAlign <float, 16> > BufAlign;

	void           update_max_block_size (int max_block_size);
	void           process_block (float** inputs, ::VstInt32 sampleFrames);
	void           process_vst_events (::VstEvents &evt_list);
	void           push_event (const mfx::piapi::EventTs &evt);
	void           fill_pin_prop (::VstPinProperties &prop, bool in_flag, int index);

	static ::VstInt32
	               gen_vst_id (const mfx::piapi::PluginDescInterface &desc);
	static ::VstIntPtr
	               vst_dispatch (::AEffect* e, ::VstInt32 opcode, ::VstInt32 index, ::VstIntPtr value, void* ptr, float opt);
	static float   vst_get_param (::AEffect* e, ::VstInt32 index);
	static void    vst_set_param (::AEffect* e, ::VstInt32 index, float value);
	static void DECLARE_VST_DEPRECATED (
	               vst_process) (::AEffect* e, float** inputs, float** outputs, ::VstInt32 sampleFrames);
	static void    vst_process_replacing (::AEffect* e, float** inputs, float** outputs, ::VstInt32 sampleFrames);

	const ::audioMasterCallback
	               _audio_master;
	const mfx::piapi::PluginDescInterface &
	               _desc;
	std::auto_ptr <mfx::piapi::PluginInterface>
	               _plugin_aptr;
	::AEffect      _vst;

	int            _nbr_i;              // Pins. Each pin can be mono or stereo
	int            _nbr_o;
	int            _nbr_s;              // Signal pins are always "mono"
	int            _latency;
	mfx::piapi::PluginInterface::ProcInfo
	               _proc;

	double         _sample_freq;        // Hz
	int            _max_block_size;
	BufAlign       _mix_buf;            // Size: _max_block_size * (_max_nbr_evt * max_number_of_(input_+_output) + signal_pins)
	std::vector <float *>
	               _src_arr;
	std::vector <float *>
	               _dst_arr;
	std::vector <float *>
	               _sig_arr;

	EventQueue     _evt_queue;
	EventPool      _evt_pool;
	std::array <mfx::piapi::EventTs *, _max_nbr_evt>
	               _evt_ptr_arr;
	std::array <EventCell *, _max_nbr_evt>
	               _cell_tmp_arr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               PlugWrap ()                               = delete;
	               PlugWrap (const PlugWrap &other)          = delete;
	PlugWrap &     operator = (const PlugWrap &other)        = delete;
	bool           operator == (const PlugWrap &other) const = delete;
	bool           operator != (const PlugWrap &other) const = delete;

}; // class PlugWrap



}  // namespace piapi2vst



//#include "piapi2vst/PlugWrap.hpp"



#endif   // piapi2vst_PlugWrap_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
