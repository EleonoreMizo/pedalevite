/*****************************************************************************

        PlugWrap.cpp
        Author: Laurent de Soras, 2018

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#if defined (_MSC_VER)
	#pragma warning (1 : 4130 4223 4705 4706)
	#pragma warning (4 : 4355 4786 4800)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "fstb/DataAlign.h"
#include "fstb/def.h"
#include "fstb/fnc.h"
#include "mfx/dsp/mix/Simd.h"
#include "mfx/pi/param/Tools.h"
#include "mfx/piapi/Dir.h"
#include "mfx/piapi/EventTs.h"
#include "mfx/piapi/FactoryInterface.h"
#include "mfx/piapi/ParamDescInterface.h"
#include "piapi2vst/PlugWrap.h"

#if defined (_MSC_VER)
#include	<crtdbg.h>
#include	<new.h>
#include	<windows.h>
#endif	// _MSC_VER

#include <cassert>
#include <cstdint>



namespace piapi2vst
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



PlugWrap::PlugWrap (audioMasterCallback audio_master, mfx::piapi::FactoryInterface &factory)
:	_audio_master (audio_master)
,	_desc (factory.describe ())
,	_plugin_aptr (factory.create ())
,	_nbr_i (1)
,	_nbr_o (1)
,	_nbr_s (0)
,	_latency (0)
,	_proc ()
,	_vst ()
,	_sample_freq (44100)
,	_max_block_size (1024)
,	_mix_buf ()
,	_src_arr ()
,	_dst_arr ()
,	_sig_arr ()
,	_evt_queue ()
,	_evt_pool ()
,	_evt_ptr_arr ()
,	_cell_tmp_arr ()
{
	_evt_pool.expand_to (_max_nbr_evt);

	const int      nbr_param =
		_desc.get_nbr_param (mfx::piapi::ParamCateg_GLOBAL);
	_desc.get_nbr_io (_nbr_i, _nbr_o, _nbr_s);

	// VST
	memset (&_vst, 0, sizeof (_vst));
	_vst.magic            = kEffectMagic;
	_vst.dispatcher       = vst_dispatch;
	_vst.DECLARE_VST_DEPRECATED (process) = DECLARE_VST_DEPRECATED (vst_process);
	_vst.setParameter     = vst_set_param;
	_vst.getParameter     = vst_get_param;
	_vst.numPrograms      = 1;
	_vst.numParams        = nbr_param;
	_vst.numInputs        = _nbr_i * _max_nbr_chn;
	_vst.numOutputs       = _nbr_o * _max_nbr_chn + _nbr_s;
	_vst.DECLARE_VST_DEPRECATED (ioRatio) = 1.f;
	_vst.object           = this;
	_vst.user             = nullptr;
	_vst.uniqueID         = gen_vst_id (_desc);
	_vst.version          = 1;
	_vst.processReplacing = vst_process_replacing;
	_vst.flags           |= ::effFlagsCanReplacing; // mandatory in VST 2.4!
	_vst.flags           |= ::DECLARE_VST_DEPRECATED (effFlagsCanMono);

#if VST_2_4_EXTENSIONS
	_vst.processDoubleReplacing = nullptr;
#endif
}



PlugWrap::~PlugWrap ()
{
	EventCell *    cell_ptr = nullptr;
	do
	{
		cell_ptr = _evt_queue.dequeue ();
		if (cell_ptr != nullptr)
		{
			_evt_pool.return_cell (*cell_ptr);
		}
	}
	while (cell_ptr != nullptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



bool	PlugWrap::EventLt::operator () (mfx::piapi::EventTs * &lhs, mfx::piapi::EventTs * &rhs)
{
	assert (lhs != nullptr);
	assert (rhs != nullptr);

	return (lhs->_timestamp < rhs->_timestamp);
}



void	PlugWrap::update_max_block_size (int max_block_size)
{
	assert (max_block_size > 0);

	_max_block_size = max_block_size;

	_src_arr.resize (_nbr_i * _max_nbr_chn);
	_dst_arr.resize (_nbr_o * _max_nbr_chn);
	_sig_arr.resize (_nbr_s);
	_proc._src_arr = nullptr;
	_proc._dst_arr = nullptr;
	_proc._sig_arr = nullptr;
	if (_nbr_i > 0)
	{
		_proc._src_arr = &_src_arr [0];
	}
	if (_nbr_o > 0)
	{
		_proc._dst_arr = &_dst_arr [0];
	}
	if (_nbr_s > 0)
	{
		_proc._sig_arr = &_sig_arr [0];
	}

	const int      mbs_alig = (max_block_size + 3) & ~3;
	_mix_buf.resize (mbs_alig * (_max_nbr_chn * (_nbr_i + _nbr_o) + _nbr_s));

	int            chn_index = 0;
	for (int chn_cnt = 0; chn_cnt < _nbr_i * _max_nbr_chn; ++chn_cnt)
	{
		_src_arr [chn_cnt] = &_mix_buf [chn_index * mbs_alig];
		++ chn_index;
	}
	for (int chn_cnt = 0; chn_cnt < _nbr_o * _max_nbr_chn; ++chn_cnt)
	{
		_dst_arr [chn_cnt] = &_mix_buf [chn_index * mbs_alig];
		++ chn_index;
	}
	for (int chn_cnt = 0; chn_cnt < _nbr_s; ++chn_cnt)
	{
		_sig_arr [chn_cnt] = &_mix_buf [chn_index * mbs_alig];
		++ chn_index;
	}
}



void	PlugWrap::process_block (float** inputs, ::VstInt32 sampleFrames)
{
	// Checks if pins are mono or stereo
	_proc._dir_arr [mfx::piapi::Dir_IN ]._nbr_chn = 1;
	_proc._dir_arr [mfx::piapi::Dir_OUT]._nbr_chn = 1;
	if (_nbr_o > 0)
	{
		const ::VstIntPtr res_o = _audio_master (
			&_vst,
			::DECLARE_VST_DEPRECATED (audioMasterPinConnected),
			1, 1, 0, 0
		);
		if (res_o == 0)
		{
			_proc._dir_arr [mfx::piapi::Dir_OUT]._nbr_chn = 2;
			if (_nbr_i > 0)
			{
				const ::VstIntPtr res_i = _audio_master (
					&_vst,
					::DECLARE_VST_DEPRECATED (audioMasterPinConnected),
					1, 0, 0, 0
				);
				if (res_i == 0)
				{
					_proc._dir_arr [mfx::piapi::Dir_IN]._nbr_chn = 2;
				}
			}
		}
	}

	// Collects and sorts the events
	int            nbr_evt  = 0;
	EventCell *    cell_ptr = nullptr;
	do
	{
		cell_ptr = _evt_queue.dequeue ();
		if (cell_ptr != nullptr)
		{
			_cell_tmp_arr [nbr_evt] = cell_ptr;
			_evt_ptr_arr [nbr_evt]  = &cell_ptr->_val;
			++ nbr_evt;
		}
	}
	while (cell_ptr != nullptr);

	if (nbr_evt == 0)
	{
		_proc._evt_arr = nullptr;
	}
	else
	{
		std::sort (
			&_evt_ptr_arr [0],
			&_evt_ptr_arr [0] + nbr_evt,
			EventLt ()
		);
		_proc._evt_arr = &_evt_ptr_arr [0];
	}
	_proc._nbr_evt = nbr_evt;

	// Copies input sample data to aligned positions
	const int      nbr_chn_per_pin =
		_proc._dir_arr [mfx::piapi::Dir_IN]._nbr_chn;
	const int      nbr_chn         = _nbr_i * nbr_chn_per_pin;
	for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
	{
		mfx::dsp::mix::Simd <
			fstb::DataAlign <true>,
			fstb::DataAlign <false>
		>::copy_1_1 (
			_src_arr [chn_cnt],
			inputs [chn_cnt],
			sampleFrames
		);
	}

	// Processing
	_proc._nbr_spl = sampleFrames;
	_plugin_aptr->process_block (_proc);

	// Returns the event cells
	for (int evt_cnt = 0; evt_cnt < nbr_evt; ++evt_cnt)
	{
		_evt_pool.return_cell (*_cell_tmp_arr [evt_cnt]);
		_cell_tmp_arr [evt_cnt] = nullptr;
	}
}



void	PlugWrap::process_vst_events (const ::VstEvents &evt_list)
{
	assert (&evt_list != nullptr);

	for (int evt_cnt = 0; evt_cnt < evt_list.numEvents; ++evt_cnt)
	{
		const ::VstEvent &   evt_gen = *(evt_list.events [evt_cnt]);
		if (evt_gen.type == ::kVstMidiType)
		{
			const ::VstMidiEvent &  evt_midi =
				reinterpret_cast <const ::VstMidiEvent &> (evt_gen);

			mfx::piapi::EventTs  evt;

			const int         status = (evt_midi.midiData [0] >> 4) & 0xF;
			const int         byte1  = uint8_t (evt_midi.midiData [1]);
			const int         byte2  = uint8_t (evt_midi.midiData [2]);

			// Note Off
			if (status == 0x8 || (status == 0x9 && evt_midi.midiData [2] == 0))
			{
				evt._type                   = mfx::piapi::EventType_NOTE_OFF;
				evt._timestamp              = evt_midi.deltaFrames;
				evt._evt._note_off._note_id = byte1;
				push_event (evt);
			}

			// Note On
			else if (status == 0x9)
			{
				evt._type                   = mfx::piapi::EventType_NOTE_ON;
				evt._timestamp              = evt_midi.deltaFrames;
				evt._evt._note_on._note_id  = byte1;	// Just use the note index.
				evt._evt._note_on._note     = float (byte1);
				push_event (evt);

				evt._type                   = mfx::piapi::EventType_PARAM;
				evt._timestamp              = evt_midi.deltaFrames;
				evt._evt._param._categ      = mfx::piapi::ParamCateg_NOTE;
				evt._evt._param._note_id    = evt_midi.midiData [1];
				evt._evt._param._index      = 0 /*** To do ***/;
				evt._evt._param._val        = float (byte2) / 128.0f;
				push_event (evt);
			}

			// Control Change
			else if (status == 0xB)
			{
				if (   evt_midi.midiData [1] == 0x78   // All Sounds Off
				    || evt_midi.midiData [1] == 0x7B)  // All Notes Off
				{
					evt._type                   = mfx::piapi::EventType_NOTE_OFF;
					evt._timestamp              = evt_midi.deltaFrames;
					evt._evt._note_off._note_id = -1;
					push_event (evt);
				}
			}
		}
	}
}



void	PlugWrap::push_event (const mfx::piapi::EventTs &evt)
{
	EventCell *    cell_ptr = _evt_pool.take_cell ();
	if (cell_ptr == nullptr)
	{
		assert (false);
	}
	else
	{
		cell_ptr->_val = evt;
		_evt_queue.enqueue (*cell_ptr);
	}
}



void	PlugWrap::fill_pin_prop (::VstPinProperties &prop, bool in_flag, int index)
{
	assert (index >= 0);

	const int      chn      = index % _max_nbr_chn;
	int            pin      = index / _max_nbr_chn;
	const bool     sig_flag = (! in_flag && pin >= _nbr_o);
	if (sig_flag)
	{
		pin -= _nbr_o;
	}
	prop.label [0]       = char ('1' + pin);
	prop.label [1]       = (sig_flag) ? 'S' : ((chn == 0) ? 'L' : 'R');
	prop.label [2]       = '\0';
	prop.shortLabel [0]  = prop.label [0];
	prop.shortLabel [1]  = prop.label [1];
	prop.shortLabel [2]  = prop.label [2];
	prop.arrangementType = ::kSpeakerArrEmpty;
	prop.flags           = ::kVstPinIsActive;
	if (sig_flag)
	{
		prop.arrangementType = ::kSpeakerArrMono;
	}
	else if (_max_nbr_chn == 2)
	{
		prop.arrangementType = ::kSpeakerArrStereo;
		if (chn == 0)
		{
			prop.flags |= ::kVstPinIsStereo;
		}
	}
}



::VstInt32	PlugWrap::gen_vst_id (const mfx::piapi::PluginDescInterface &desc)
{
	const mfx::piapi::PluginInfo pi_info { desc.get_info () };

	// Simple hash value
	uint32_t       hash = 1;
	for (auto c : pi_info._unique_id)
	{
		hash *= uint8_t (c);
	}

	// Converts to a readable 4-char code
	::VstInt32     vst_id = 0;
	for (int k = 0; k < 4; ++k)
	{
		uint8_t        code = hash & 31;
		if (code < 10)
		{
			code += '0';
		}
		else
		{
			code -= 10;
			code += 'a';
		}

		vst_id <<= 8;
		vst_id  += code;
		hash   >>= 5;
	}

	return vst_id;
}



::VstIntPtr	PlugWrap::vst_dispatch (::AEffect* e, ::VstInt32 opcode, ::VstInt32 index, ::VstIntPtr value, void* ptr, float opt)
{
	PlugWrap *     wrapper_ptr = static_cast <PlugWrap *> (e->object);
	assert (wrapper_ptr != nullptr);

	mfx::piapi::PluginInterface & plugin = *(wrapper_ptr->_plugin_aptr);
		
	int            ret_val = 0;

	std::string    txt;
	std::string    result;
	double         param_val = 0;
	double         param_nat = 0;

	switch (opcode)
	{
	case ::effClose:
		delete wrapper_ptr;
		ret_val = 1;
		break;

	case ::effGetProgramName:
		strcpy (static_cast <char *> (ptr), "Default"); 
		break;

	case ::effGetParamLabel:
		{
			const mfx::piapi::PluginDescInterface &   pi_desc = wrapper_ptr->_desc;
			const mfx::piapi::ParamDescInterface & desc =
				pi_desc.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
			txt = desc.get_unit (::kVstMaxParamStrLen);
			fstb::snprintf4all (
				reinterpret_cast <char *> (ptr),
				::kVstMaxParamStrLen + 1,
				"%s",
				txt.c_str ()
			);
		}
		break;
	case ::effGetParamDisplay:
		{
			const mfx::piapi::PluginDescInterface &   pi_desc = wrapper_ptr->_desc;
			const mfx::piapi::ParamDescInterface & desc =
				pi_desc.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
			param_val = plugin.get_param_val (mfx::piapi::ParamCateg_GLOBAL, index, 0);
			param_nat = desc.conv_nrm_to_nat (param_val);
			txt = desc.conv_nat_to_str (param_nat, ::kVstMaxParamStrLen);
			fstb::snprintf4all (
				reinterpret_cast <char *> (ptr),
				::kVstMaxParamStrLen + 1,
				"%s",
				txt.c_str ()
			);
		}
		break;
	case ::effGetParamName:
		{
			const mfx::piapi::PluginDescInterface &   pi_desc = wrapper_ptr->_desc;
			const mfx::piapi::ParamDescInterface & desc =
				pi_desc.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
			txt = desc.get_name (::kVstMaxParamStrLen);
			fstb::snprintf4all (
				reinterpret_cast <char *> (ptr),
				::kVstMaxParamStrLen + 1,
				"%s",
				txt.c_str ()
			);
		}
		break;
	case ::effSetSampleRate:
		wrapper_ptr->_sample_freq = opt;
		break;
	case ::effSetBlockSize:
		wrapper_ptr->update_max_block_size (int (value));
		break;
	case ::effMainsChanged:
		if (value != 0)
		{
			wrapper_ptr->_latency = 0;
			plugin.reset (
				wrapper_ptr->_sample_freq,
				wrapper_ptr->_max_block_size,
				wrapper_ptr->_latency
			);
		}
		break;
	case ::effProcessEvents:
		wrapper_ptr->process_vst_events (*static_cast < ::VstEvents *> (ptr));
		break;

	case ::effCanBeAutomated:
		if (index >= 0 && index < wrapper_ptr->_vst.numParams)
		{
			ret_val = 1;
		}
		break;

	case ::effString2Parameter:
		if (index >= 0 && index < wrapper_ptr->_vst.numParams)
		{
			if (ptr == nullptr)
			{
				ret_val = 1;
			}
			else
			{
				const mfx::piapi::PluginDescInterface &   pi_desc = wrapper_ptr->_desc;
				const mfx::piapi::ParamDescInterface & desc =
					pi_desc.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
				double         val_nat = 0;
				const bool     ok_flag = desc.conv_str_to_nat (
					val_nat, static_cast <const char *> (ptr)
				);
				if (ok_flag)
				{
					const double   val_nrm = desc.conv_nat_to_nrm (val_nat);
					vst_set_param (&wrapper_ptr->_vst, index, float (val_nrm));
					ret_val = 1;
				}
			}
		}
		break;

	case ::DECLARE_VST_DEPRECATED (effGetInputProperties):
		wrapper_ptr->fill_pin_prop (
			*static_cast <::VstPinProperties *> (ptr), true, index
		);
		ret_val = 1;
		break;

	case ::DECLARE_VST_DEPRECATED (effGetOutputProperties):
		wrapper_ptr->fill_pin_prop (
			*static_cast <::VstPinProperties *> (ptr), false, index
		);
		ret_val = 1;
		break;

	case ::effGetVendorString:
		fstb::snprintf4all (
			static_cast <char *> (ptr), ::kVstMaxVendorStrLen,
			"%s", "Pedale Vite plug-ins"
		);
		ret_val = 1;
		break;

	case ::effGetProductString:
		{
			const mfx::piapi::PluginDescInterface &   pi_desc = wrapper_ptr->_desc;
			const mfx::piapi::PluginInfo pi_info { pi_desc.get_info () };
			const std::string   name = mfx::pi::param::Tools::print_name_bestfit (
				::kVstMaxProductStrLen - 1, pi_info._name.c_str ()
			);
			fstb::snprintf4all (
				reinterpret_cast <char *> (ptr), ::kVstMaxProductStrLen,
				"%s", name.c_str ()
			);
			ret_val = 1;
		}
		break;

	case ::effCanDo:
		{
			const char *   c_ptr = static_cast <const char *> (ptr);
			if (   strcmp (c_ptr, "receiveVstEvents"   ) == 0
			    || strcmp (c_ptr, "receiveVstMidiEvent") == 0
			    || strcmp (c_ptr, "receiveVstTimeInfo" ) == 0)
			{
				ret_val = 1;
			}
		}
		break;

	case ::effGetParameterProperties:
		{
			::VstParameterProperties & vst_prop =
				*static_cast < ::VstParameterProperties *> (ptr);
			memset (&vst_prop, 0, sizeof (vst_prop));
			const mfx::piapi::PluginDescInterface &   pi_desc = wrapper_ptr->_desc;
			const mfx::piapi::ParamDescInterface & desc =
				pi_desc.get_param_info (mfx::piapi::ParamCateg_GLOBAL, index);
			txt = desc.get_name (::kVstMaxLabelLen);
			fstb::snprintf4all (
				vst_prop.label,
				::kVstMaxLabelLen + 1,
				"%s",
				txt.c_str ()
			);
			txt = desc.get_name (::kVstMaxShortLabelLen);
			fstb::snprintf4all (
				vst_prop.shortLabel,
				::kVstMaxShortLabelLen + 1,
				"%s",
				txt.c_str ()
			);
			if (desc.get_range () == mfx::piapi::ParamDescInterface::Range_DISCRETE)
			{
				vst_prop.flags     |=   ::kVstParameterUsesIntegerMinMax
				                      | ::kVstParameterUsesIntStep;
				vst_prop.minInteger =
					::VstInt32 (fstb::round_int (desc.get_nat_min ()));
				vst_prop.maxInteger =
					::VstInt32 (fstb::round_int (desc.get_nat_max ()));
				vst_prop.stepInteger      = 1;
				vst_prop.largeStepInteger = 1;
				if (vst_prop.minInteger == 0 && vst_prop.maxInteger == 1)
				{
					vst_prop.flags |= ::kVstParameterIsSwitch;
				}
			}

			ret_val = 1;
		}
		break;
	}

	return (ret_val);
}



float	PlugWrap::vst_get_param (::AEffect* e, ::VstInt32 index)
{
	const PlugWrap * const  wrapper_ptr =
		static_cast <const PlugWrap *> (e->object);
	assert (wrapper_ptr != nullptr);

	const mfx::piapi::PluginInterface & plugin = *(wrapper_ptr->_plugin_aptr);

	return float (
		plugin.get_param_val (mfx::piapi::ParamCateg_GLOBAL, index, 0)
	);
}



void	PlugWrap::vst_set_param (::AEffect* e, ::VstInt32 index, float value)
{
	PlugWrap *  const wrapper_ptr = static_cast <PlugWrap *> (e->object);
	assert (wrapper_ptr != nullptr);

	EventCell *    cell_ptr = wrapper_ptr->_evt_pool.take_cell ();
	if (cell_ptr == nullptr)
	{
		assert (false);
	}
	else
	{
		cell_ptr->_val._timestamp         = 0;
		cell_ptr->_val._type              = mfx::piapi::EventType_PARAM;
		cell_ptr->_val._evt._param._categ = mfx::piapi::ParamCateg_GLOBAL;
		cell_ptr->_val._evt._param._index = index;
		cell_ptr->_val._evt._param._val   = value;
		wrapper_ptr->_evt_queue.enqueue (*cell_ptr);
	}
}



void	PlugWrap::DECLARE_VST_DEPRECATED (vst_process) (::AEffect* e, float** inputs, float** outputs, ::VstInt32 sampleFrames)
{
	PlugWrap * const  wrapper_ptr = static_cast <PlugWrap *> (e->object);
	assert (wrapper_ptr != nullptr);

	wrapper_ptr->process_block (inputs, sampleFrames);

	// Mixes output sample data
	const int      nbr_chn_per_pin =
		wrapper_ptr->_proc._dir_arr [mfx::piapi::Dir_OUT]._nbr_chn;
	const int      nbr_chn = wrapper_ptr->_nbr_o * nbr_chn_per_pin;
	for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
	{
		mfx::dsp::mix::Simd <
			fstb::DataAlign <false>,
			fstb::DataAlign <true>
		>::mix_1_1 (
			outputs [chn_cnt],
			wrapper_ptr->_dst_arr [chn_cnt],
			sampleFrames
		);
	}
}



void	PlugWrap::vst_process_replacing (::AEffect* e, float** inputs, float** outputs, ::VstInt32 sampleFrames)
{
	PlugWrap * const  wrapper_ptr = static_cast <PlugWrap *> (e->object);
	assert (wrapper_ptr != nullptr);

	wrapper_ptr->process_block (inputs, sampleFrames);

	// Copies output sample data
	const int      nbr_chn_per_pin =
		wrapper_ptr->_proc._dir_arr [mfx::piapi::Dir_OUT]._nbr_chn;
	const int      nbr_chn = wrapper_ptr->_nbr_o * nbr_chn_per_pin;
	for (int chn_cnt = 0; chn_cnt < nbr_chn; ++chn_cnt)
	{
		mfx::dsp::mix::Simd <
			fstb::DataAlign <false>,
			fstb::DataAlign <true>
		>::copy_1_1 (
			outputs [chn_cnt],
			wrapper_ptr->_dst_arr [chn_cnt],
			sampleFrames
		);
	}
}



}  // namespace piapi2vst



/*\\\ FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



#if defined (_MSC_VER)
static int __cdecl	piapi2vst_PlugWrap_new_handler_cb (size_t dummy)
{
	fstb::unused (dummy);

	throw std::bad_alloc ();
	return 0;
}
#endif	// _MSC_VER



#if defined (_MSC_VER) && ! defined (NDEBUG)
static int	__cdecl	piapi2vst_PlugWrap_debug_alloc_hook_cb (int alloc_type, void *user_data_ptr, size_t size, int block_type, long request_nbr, const unsigned char *filename_0, int line_nbr)
{
	fstb::unused (user_data_ptr, size, request_nbr, filename_0, line_nbr);

	if (block_type != _CRT_BLOCK)	// Ignore CRT blocks to prevent infinite recursion
	{
		switch (alloc_type)
		{
		case _HOOK_ALLOC:
		case _HOOK_REALLOC:
		case _HOOK_FREE:

			// Put some debug code here

			break;

		default:
			assert (false);	// Undefined allocation type
			break;
		}
	}

	return (1);
}
#endif



#if defined (_MSC_VER) && ! defined (NDEBUG)
static int	__cdecl	piapi2vst_PlugWrap_debug_report_hook_cb (int report_type, char *user_msg_0, int *ret_val_ptr)
{
	fstb::unused (user_msg_0);

	*ret_val_ptr = 0;	// 1 to override the CRT default reporting mode

	switch (report_type)
	{
	case _CRT_WARN:
	case _CRT_ERROR:
	case _CRT_ASSERT:

// Put some debug code here

		break;
	}

	return (*ret_val_ptr);
}
#endif



static void	piapi2vst_PlugWrap_main_prog_init ()
{
#if defined (_MSC_VER)
	::_set_new_handler (::piapi2vst_PlugWrap_new_handler_cb);
#endif	// _MSC_VER

#if defined (_MSC_VER) && ! defined (NDEBUG)
	{
		const int	mode =   (1 * _CRTDBG_MODE_DEBUG)
						       | (1 * _CRTDBG_MODE_WNDW);
		::_CrtSetReportMode (_CRT_WARN, mode);
		::_CrtSetReportMode (_CRT_ERROR, mode);
		::_CrtSetReportMode (_CRT_ASSERT, mode);

		const int	old_flags = ::_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG);
		::_CrtSetDbgFlag (  old_flags
		                  | (1 * _CRTDBG_LEAK_CHECK_DF)
		                  | (1 * _CRTDBG_CHECK_ALWAYS_DF));
		::_CrtSetBreakAlloc (-1);	// Specify here a memory bloc number
		::_CrtSetAllocHook (piapi2vst_PlugWrap_debug_alloc_hook_cb);
		::_CrtSetReportHook (piapi2vst_PlugWrap_debug_report_hook_cb);

		// Speed up I/O but breaks C stdio compatibility
//		std::cout.sync_with_stdio (false);
//		std::cin.sync_with_stdio (false);
//		std::cerr.sync_with_stdio (false);
//		std::clog.sync_with_stdio (false);
	}
#endif	// _MSC_VER, NDEBUG
}



static void	piapi2vst_PlugWrap_main_prog_end ()
{
#if defined (_MSC_VER) && ! defined (NDEBUG)
	{
		const int	mode =   (1 * _CRTDBG_MODE_DEBUG)
						       | (0 * _CRTDBG_MODE_WNDW);
		::_CrtSetReportMode (_CRT_WARN, mode);
		::_CrtSetReportMode (_CRT_ERROR, mode);
		::_CrtSetReportMode (_CRT_ASSERT, mode);

		::_CrtMemState	mem_state;
		::_CrtMemCheckpoint (&mem_state);
		::_CrtMemDumpStatistics (&mem_state);
	}
#endif	// _MSC_VER, NDEBUG
}



// Currently disabled because stumbling on:
// error LNK2005: _DllMain@12 already defined in MSVCRTD.lib(dllmain.obj)
#if 0 
#if defined (_MSC_VER)

::BOOL WINAPI DllMain (::HINSTANCE hinst, ::DWORD reason, ::LPVOID reserved_ptr)
{
	switch (reason)
	{ 
	case	DLL_PROCESS_ATTACH:
		piapi2vst_PlugWrap_main_prog_init ();
		break;

	// Let's destroy here any created singleton
	case	DLL_PROCESS_DETACH:
		piapi2vst_PlugWrap_main_prog_end ();
		break;
	}

	return (TRUE);
}

#endif	// _MSC_VER
#endif



// Partially copied from vstpluginmain.cpp:
extern "C"
{

#if defined (__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
	#define VST_EXPORT	__attribute__ ((visibility ("default")))
#else
	#define VST_EXPORT
#endif

//------------------------------------------------------------------------
/** Prototype of the export function main */
//------------------------------------------------------------------------
VST_EXPORT static ::AEffect * piapi2vst_PlugWrap_create_vst (::audioMasterCallback audioMaster)
{
	// Get VST Version of the Host
	if (! audioMaster (nullptr, ::audioMasterVersion, 0, 0, nullptr, 0))
	{
		return nullptr;  // old version
	}

	// Creates the plug-in
	::AEffect *    vst_ptr = nullptr;
	bool           ok_flag = true;

	try
	{
		piapi2vst::PlugWrap *   fx_ptr =
			new piapi2vst::PlugWrap (audioMaster, main_use_plugin_factory ());
		vst_ptr = &fx_ptr->use_aeffect ();
	}
	catch (...)
	{
		ok_flag = false;
	}

	assert (ok_flag);

	// Returns the VST AEffect structure
	return vst_ptr;
}

// support for old hosts not looking for VSTPluginMain

// here we make a little patch to ensure that main_macho is
// defined in both intel & ppc part of the binary for linking
//#if (TARGET_API_MAC_CARBON && __ppc__)
#if (TARGET_API_MAC_CARBON)
VST_EXPORT ::AEffect * main_macho (::audioMasterCallback audioMaster)
{
	return piapi2vst_PlugWrap_create_vst (audioMaster);
}
#elif WIN32
VST_EXPORT ::AEffect * main_vst (::audioMasterCallback audioMaster)
{
	return piapi2vst_PlugWrap_create_vst (audioMaster);
}
#elif BEOS
VST_EXPORT ::AEffect * main_plugin (::audioMasterCallback audioMaster)
{
	return piapi2vst_PlugWrap_create_vst (audioMaster);
}
#endif

} // extern "C"



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
