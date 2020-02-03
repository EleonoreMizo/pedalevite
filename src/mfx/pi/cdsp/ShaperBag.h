/*****************************************************************************

        ShaperBag.h
        Author: Laurent de Soras, 2020

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_pi_cdsp_ShaperBag_HEADER_INCLUDED)
#define mfx_pi_cdsp_ShaperBag_HEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "mfx/dsp/shape/FncFiniteAsym.h"
#include "mfx/dsp/shape/FncLin0.h"
#include "mfx/dsp/shape/FncScale.h"
#include "mfx/dsp/shape/WsAsinh.h"
#include "mfx/dsp/shape/WsAsym2.h"
#include "mfx/dsp/shape/WsAtan.h"
#include "mfx/dsp/shape/WsBreakBase.h"
#include "mfx/dsp/shape/WsLopsided.h"
#include "mfx/dsp/shape/WsOvershootAsym.h"
#include "mfx/dsp/shape/WsProgClipper.h"
#include "mfx/dsp/shape/WsPuncherA.h"
#include "mfx/dsp/shape/WsPuncherB.h"
#include "mfx/dsp/shape/WsSmartE.h"
#include "mfx/dsp/shape/WsTanh.h"



namespace mfx
{
namespace pi
{
namespace cdsp
{



class ShaperBag
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	template <class FNC>
	using ShaperLong = dsp::shape::FncFiniteAsym <
		-256, 256, FNC
	>;

	template <class FNC>
	using ShaperStd = dsp::shape::FncFiniteAsym <
		-20, 20, FNC, 2
	>;

	template <class FNC>
	using ShaperRes = dsp::shape::FncFiniteAsym <
		-16, 16, FNC, 4
	>;

	template <class FNC>
	using ShaperShort = dsp::shape::FncFiniteAsym <
		-8, 8, FNC, 4
	>;

	typedef ShaperShort <dsp::shape::WsTanh>  ShaperTanh;
	typedef ShaperShort <dsp::shape::FncLin0 <
		dsp::shape::WsTanh, std::ratio <1, 2>
	> > ShaperTanhLin;
	typedef ShaperShort <dsp::shape::FncLin0 <
		dsp::shape::WsBreakBase, std::ratio <3, 4>
	> > ShaperBreak;
	typedef ShaperStd <  dsp::shape::WsAtan>  ShaperAtan;
	typedef ShaperStd <  dsp::shape::FncScale <
		dsp::shape::WsAsinh,
		std::ratio <1, 4>
	> > ShaperDiode;
	typedef ShaperLong < dsp::shape::WsAsinh> ShaperAsinh;
	typedef ShaperStd <  dsp::shape::WsProgClipper <
		std::ratio < 2, 4>,
		std::ratio < 4, 1>,
		std::ratio < 2, 1>
	> > ShaperProg1;
	typedef ShaperStd <dsp::shape::WsProgClipper <
		std::ratio < 3, 4>,
		std::ratio <10, 1>,
		std::ratio < 0, 1>
	> > ShaperProg2;
	typedef ShaperStd <  dsp::shape::WsPuncherB <0> > ShaperPuncher1;
	typedef ShaperStd <  dsp::shape::WsPuncherB <1> > ShaperPuncher2;
	typedef ShaperShort <dsp::shape::WsPuncherA     > ShaperPuncher3;
	typedef ShaperShort <dsp::shape::WsOvershootAsym> ShaperOvershoot;
	typedef ShaperShort <dsp::shape::WsLopsided     > ShaperLopsided;
	typedef ShaperRes <  dsp::shape::WsSmartE <
		std::ratio <-86, 100>,
		std::ratio <  1,   1> >
	> ShaperSmartE1;
	typedef ShaperShort <dsp::shape::WsSmartE <
		std::ratio <-86, 100>,
		std::ratio <  2,   1> >
	> ShaperSmartE2;
	typedef dsp::shape::FncFiniteAsym <
		-256, 8, dsp::shape::WsAsym2, 2
	> ShaperAsym2;

	virtual        ~ShaperBag () = default;

	static ShaperBag &
	               use ();

	ShaperTanh     _s_tanh;
	ShaperAtan     _s_atan;
	ShaperAsinh    _s_asinh;
	ShaperDiode    _s_diode;
	ShaperProg1    _s_prog1;
	ShaperProg2    _s_prog2;
	ShaperPuncher1 _s_puncher1;
	ShaperPuncher2 _s_puncher2;
	ShaperPuncher3 _s_puncher3;
	ShaperOvershoot
	               _s_overshoot;
	ShaperLopsided _s_lopsided;
	ShaperSmartE1  _s_smarte1;
	ShaperSmartE2  _s_smarte2;
	ShaperTanhLin  _s_tanhlin;
	ShaperBreak    _s_break;
	ShaperAsym2    _s_asym2;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ShaperBag () = default;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               ShaperBag (const ShaperBag &other)         = delete;
	               ShaperBag (ShaperBag &&other)              = delete;
	ShaperBag &    operator = (const ShaperBag &other)        = delete;
	ShaperBag &    operator = (ShaperBag &&other)             = delete;
	bool           operator == (const ShaperBag &other) const = delete;
	bool           operator != (const ShaperBag &other) const = delete;

}; // class ShaperBag



}  // namespace cdsp
}  // namespace pi
}  // namespace mfx



//#include "mfx/pi/cdsp/ShaperBag.hpp"



#endif   // mfx_pi_cdsp_ShaperBag_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

