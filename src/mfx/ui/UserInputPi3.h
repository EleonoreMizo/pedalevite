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
#if ! defined (mfx_ui_UserInputPi3_HEADER_INCLUDED)
#define mfx_ui_UserInputPi3_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "conc/CellPool.h"
#include "mfx/ui/RotEnc.h"
#include "mfx/ui/UserInputInterface.h"

#include <array>
#include <mutex>
#include <thread>
#include <vector>



#undef mfx_ui_UserInputPi3_NEW_BOARD



namespace mfx
{
namespace ui
{



class UserInputPi3
:	public UserInputInterface
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
	};

	static const int  _antibounce_time = 30 * 1000*1000; // Nanoseconds

#if defined (mfx_ui_UserInputPi3_NEW_BOARD)
	static const int  _nbr_dev_23017   = 2;
#else
	static const int  _nbr_dev_23017   = 1;
#endif
	static const int  _i2c_dev_23017_arr [_nbr_dev_23017];
	static const int  _nbr_sw_23017    = 16;            // Inputs per device

	static const int  _nbr_sw_gpio     = 2;
	static const int  _gpio_pin_arr [_nbr_sw_gpio];

	static const int  _nbr_adc         = 8;
	static const int  _res_adc         = 10;            // Bits
	static const int  _spi_port        = 0;             // For the ADC
	static const int  _spi_rate        = 1 * 1000*1000; // Hz

#if defined (mfx_ui_UserInputPi3_NEW_BOARD)
	static const int  _nbr_switches    = _nbr_sw_gpio + _nbr_sw_23017 + 2;
#else
	static const int  _nbr_switches    = _nbr_sw_gpio + _nbr_sw_23017;
#endif
	static const SwitchSrc
	                  _switch_arr [_nbr_switches];

#if defined (mfx_ui_UserInputPi3_NEW_BOARD)
	static const int  _nbr_rotenc      = 7;
	static const RotEncSrc
	                  _rotenc_arr [_nbr_rotenc];
#else
	static const int  _nbr_rotenc      = 0;
#endif

#if defined (mfx_ui_UserInputPi3_NEW_BOARD)
	static const int  _nbr_pot         = 3;
#else
	static const int  _nbr_pot         = 1;
#endif
	static const int  _pot_arr [_nbr_pot];

	explicit       UserInputPi3 (std::mutex &mutex_spi);
	virtual        ~UserInputPi3 ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:

	// UserInputInterface
	virtual int    do_get_nbr_param (UserInputType type) const;
	virtual void   do_set_msg_recipient (UserInputType type, int index, MsgQueue *queue_ptr);
	virtual void   do_return_cell (MsgCell &cell);



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	// MCP23017 registers
	// IOCON.BANK = 0
	enum Cmd23017 : uint8_t
	{
		Cmd23017_IODIRA = 0x00,
		Cmd23017_IODIRB,
		Cmd23017_IPOLA,
		Cmd23017_IPOLB,
		Cmd23017_GPINTENA,
		Cmd23017_GPINTENB,
		Cmd23017_DEFVALA,
		Cmd23017_DEFVALB,
		Cmd23017_INTCONA,
		Cmd23017_INTCONB,
		Cmd23017_IOCONA,
		Cmd23017_IOCONB,
		Cmd23017_GPPUA,
		Cmd23017_GPPUB,
		Cmd23017_INTFA,
		Cmd23017_INTFB,
		Cmd23017_INTCAPA,
		Cmd23017_INTCAPB,
		Cmd23017_GPIOA,
		Cmd23017_GPIOB,
		Cmd23017_OLATA,
		Cmd23017_OLATB
	};
	enum IOCon : uint8_t
	{
		IOCon_BANK   = 0x80,
		IOCon_MIRROR = 0x40,
		IOCon_SEQOP  = 0x20,
		IOCon_DISSLW = 0x10,
		IOCon_HAEN   = 0x08,
		IOCon_ODR    = 0x04,
		IOCon_INTPOL = 0x02
	};

	class SwitchState
	{
	public:
		bool           _flag      = false;
		int64_t        _time_last = 0;
	};
	typedef std::array <SwitchState, _nbr_switches> SwitchStateArray;

	class PotState
	{
	public:
		static const int  _val_none = -666;
		int            _cur_val = _val_none;
		int            _alt_val = _val_none;
	};
	typedef std::array <PotState, _nbr_adc> PotStateArray;

	typedef std::array <RotEnc, _nbr_rotenc>   RotEncStateArray;

	typedef conc::CellPool <UserInputMsg> MsgPool;
	typedef std::vector <MsgQueue *> QueueArray;
	typedef std::array <QueueArray, UserInputType_NBR_ELT> RecipientList;

	void           close_everything ();
	void           polling_loop ();
	void           handle_switch (int index, bool flag, int64_t cur_time);
	void           handle_rotenc (int index, bool f0, bool f1, int64_t cur_time);
	void           handle_pot (int index, int val, int64_t cur_time);
	void           enqueue_val (UserInputType type, int index, float val);
	int            read_adc (int port, int chn);
	int64_t        read_clock_ns () const;

	std::mutex &   _mutex_spi;
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

	volatile bool  _quit_flag;
	std::thread    _polling_thread;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               UserInputPi3 ()                               = delete;
	               UserInputPi3 (const UserInputPi3 &other)      = delete;
	UserInputPi3 & operator = (const UserInputPi3 &other)        = delete;
	bool           operator == (const UserInputPi3 &other) const = delete;
	bool           operator != (const UserInputPi3 &other) const = delete;

}; // class UserInputPi3



}  // namespace ui
}  // namespace mfx



//#include "mfx/ui/UserInputPi3.hpp"



#endif   // mfx_ui_UserInputPi3_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
