/*****************************************************************************

        GpioAccess.hpp
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#if ! defined (mfx_hw_GpioAccess_CODEHEADER_INCLUDED)
#define mfx_hw_GpioAccess_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <cassert>



namespace mfx
{
namespace hw
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	GpioAccess::clear (int gpio) const
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	const uint32_t mask = uint32_t (1) << gpio;
	_gpio_ptr [_ofs_reg_clr] = mask;
}



void	GpioAccess::set (int gpio) const
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	const uint32_t mask = uint32_t (1) << gpio;
	_gpio_ptr [_ofs_reg_set] = mask;
}



void	GpioAccess::write (int gpio, int val) const
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	if (val == 0)
	{
		clear (gpio);
	}
	else
	{
		set (gpio);
	}
}



int	GpioAccess::read (int gpio) const
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	_last_read = _gpio_ptr [_ofs_reg_lvl];

	return read_cached (gpio);
}



int	GpioAccess::read_cached (int gpio) const
{
	return (_last_read >> gpio) & 1;
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	GpioAccess::find_addr_fnc (int &ofs_reg, int &shf_bit, int gpio)
{
	assert (gpio >= 0);
	assert (gpio < _nbr_gpio);

	const int      nbr_gpio_per_reg = 10;

	ofs_reg =  gpio / nbr_gpio_per_reg;
	shf_bit = (gpio - (ofs_reg * nbr_gpio_per_reg)) * _fnc_field_size;
}



}  // namespace hw
}  // namespace mfx



#endif   // mfx_hw_GpioAccess_CODEHEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
