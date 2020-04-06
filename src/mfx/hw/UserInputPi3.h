/*****************************************************************************

        UserInputPi3.h
        Author: Laurent de Soras, 2016

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://sam.zoy.org/wtfpl/COPYING for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_UserInputPi3_HEADER_INCLUDED)
#define mfx_hw_UserInputPi3_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/CellPool.h"
#include "mfx/ui/RotEnc.h"
#include "mfx/ui/TimeShareCbInterface.h"
#include "mfx/ui/UserInputInterface.h"
#include "mfx/Cst.h"

#include <array>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>



namespace mfx
{

namespace ui
{
class TimeShareThread;
}

namespace hw
{



class UserInputPi3 final
:	public ui::UserInputInterface
,	public ui::TimeShareCbInterface
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum BinSrc
	{
		BinSrc_GPIO = 0,
		BinSrc_PORT_EXP,

		BinSrc_NBR_ELT
	};

	class SwitchSrc
	{
	public:
		BinSrc         _type;
		int            _pos;
	};
	class RotEncSrc
	{
	public:
		BinSrc         _type;
		int            _pos_0;
		int            _pos_1;
		int            _dir_mul; // 1 or -1
	};

	static const std::chrono::nanoseconds               // Nanoseconds
	                  _antibounce_time;

	static const int  _nbr_dev_23017   = 2;
	static const int  _i2c_dev_23017_arr [_nbr_dev_23017];
	static const int  _nbr_sw_23017    = 16;            // Inputs per device

	static const int  _nbr_sw_gpio     = 2;
	static const int  _gpio_pin_arr [_nbr_sw_gpio];

	static const int  _nbr_adc         = 8;
	static const int  _res_adc         = 10;            // Bits
	static const int  _spi_port        = 0;             // For the ADC
	static const int  _spi_rate        = 1 * 1000*1000; // Hz

	static const int  _nbr_switches    = _nbr_sw_gpio + _nbr_sw_23017 + 2;
	static const SwitchSrc
	                  _switch_arr [_nbr_switches];

	static const RotEncSrc
	                  _rotenc_arr [Cst::RotEnc_NBR_ELT];

	static const int  _pot_arr [Cst::_nbr_pot];

	explicit       UserInputPi3 (ui::TimeShareThread &thread_spi);
	virtual        ~UserInputPi3 ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// UserInputInterface
	int            do_get_nbr_param (ui::UserInputType type) const final;
	void           do_set_msg_recipient (ui::UserInputType type, int index, MsgQueue *queue_ptr) final;
	void           do_return_cell (MsgCell &cell) final;
	std::chrono::microseconds
	               do_get_cur_date () const final;

	// TimeShareCbInterface
	bool           do_process_timeshare_op () final;



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	class SwitchState
	{
	public:
		               SwitchState ();
		std::atomic <bool>
			            _flag;
		std::chrono::nanoseconds
		               _time_last; // ns
		bool           is_set () const;
	};
	typedef std::array <SwitchState, _nbr_switches> SwitchStateArray;

	class PotState
	{
	public:
		static const int  _val_none = -666;
		std::atomic <int>
		               _cur_val = _val_none;
		int            _alt_val = _val_none;
		bool           is_set () const { return _cur_val != _val_none; }
	};
	typedef std::array <PotState, _nbr_adc> PotStateArray;

	typedef std::array <ui::RotEnc, Cst::RotEnc_NBR_ELT>   RotEncStateArray;

	typedef conc::CellPool <ui::UserInputMsg> MsgPool;
	typedef std::vector <MsgQueue *> QueueArray;
	typedef std::array <QueueArray, ui::UserInputType_NBR_ELT> RecipientList;

	void           close_everything ();
	void           polling_loop ();
	void           read_data (bool low_freq_flag);
	void           handle_switch (int index, bool flag, std::chrono::nanoseconds cur_time);
	void           handle_rotenc (int index, bool f0, bool f1, std::chrono::nanoseconds cur_time);
	void           handle_pot (int index, int val, std::chrono::nanoseconds cur_time);
	void           enqueue_val (std::chrono::nanoseconds date, ui::UserInputType type, int index, float val);
	int            read_adc (int port, int chn);
	std::chrono::nanoseconds
	               read_clock_ns () const;

	ui::TimeShareThread &
	               _thread_spi;
	std::array <int, _nbr_dev_23017>
	               _hnd_23017_arr;      // MCP23017: Port expander
	int            _hnd_3008;           // MCP3008 : ADC

	RecipientList  _recip_list;
	SwitchStateArray
	               _switch_state_arr;
	PotStateArray  _pot_state_arr;
	RotEncStateArray
	               _rotenc_state_arr;

	MsgPool        _msg_pool;

	std::atomic <bool>
	               _quit_flag;
	std::thread    _polling_thread;
	int            _polling_count;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               UserInputPi3 ()                               = delete;
	               UserInputPi3 (const UserInputPi3 &other)      = delete;
	               UserInputPi3 (UserInputPi3 &&other)           = delete;
	UserInputPi3 & operator = (const UserInputPi3 &other)        = delete;
	UserInputPi3 & operator = (UserInputPi3 &&other)             = delete;
	bool           operator == (const UserInputPi3 &other) const = delete;
	bool           operator != (const UserInputPi3 &other) const = delete;

}; // class UserInputPi3



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/UserInputPi3.hpp"



#endif   // mfx_ui_UserInputPi3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
