/*****************************************************************************

        mcp23017.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_hw_mcp23017_mcp23017_HEADER_INCLUDED)
#define mfx_hw_mcp23017_mcp23017_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{
namespace hw
{
namespace mcp23017
{


// MCP23017 registers
// IOCON.BANK = 0
static const int  cmd_iodira   = 0x00;
static const int  cmd_iodirb   = 0x01;
static const int  cmd_ipola    = 0x02;
static const int  cmd_ipolb    = 0x03;
static const int  cmd_gpintena = 0x04;
static const int  cmd_gpintenb = 0x05;
static const int  cmd_defvala  = 0x06;
static const int  cmd_defvalb  = 0x07;
static const int  cmd_intcona  = 0x08;
static const int  cmd_intconb  = 0x09;
static const int  cmd_iocona   = 0x0A;
static const int  cmd_ioconb   = 0x0B;
static const int  cmd_gppua    = 0x0C;
static const int  cmd_gppub    = 0x0D;
static const int  cmd_intfa    = 0x0E;
static const int  cmd_intfb    = 0x0F;
static const int  cmd_intcapa  = 0x10;
static const int  cmd_intcapb  = 0x11;
static const int  cmd_gpioa    = 0x12;
static const int  cmd_gpiob    = 0x13;
static const int  cmd_olata    = 0x14;
static const int  cmd_olatb    = 0x15;

static const int  iocon_bank   = 0x80;
static const int  iocon_mirror = 0x40;
static const int  iocon_seqop  = 0x20;
static const int  iocon_disslw = 0x10;
static const int  iocon_haen   = 0x08;
static const int  iocon_odr    = 0x04;
static const int  iocon_intpol = 0x02;



}  // namespace mcp23017
}  // namespace hw
}  // namespace mfx



//#include "mfx/hw/mcp23017/mcp23017.hpp"



#endif   // mfx_hw_mcp23017_mcp23017_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
