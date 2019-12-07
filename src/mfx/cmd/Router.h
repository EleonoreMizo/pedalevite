/*****************************************************************************

        Router.h
        Author: Laurent de Soras, 2019

--- Legal stuff ---

This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.

*Tab=3***********************************************************************/



#pragma once
#if ! defined (mfx_cmd_Router_HEADER_INCLUDED)
#define mfx_cmd_Router_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma warning (4 : 4250)
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace mfx
{

class PluginPool;

namespace cmd
{



class Document;

class Router
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	               Router ()  = default;
	virtual        ~Router () = default;

	void           set_process_info (double sample_freq, int max_block_size);
	void           create_routing (Document &doc, PluginPool &plugin_pool);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	void           create_routing_chain (Document &doc, PluginPool &plugin_pool);

	// Sampling rate, Hz. > 0. 0 = not known yet
	double         _sample_freq    = 0;

	// Maximum processing length, samples. > 0. 0 = not known yet
	int            _max_block_size = 0;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	               Router (const Router &other)            = delete;
	Router &       operator = (const Router &other)        = delete;
	bool           operator == (const Router &other) const = delete;
	bool           operator != (const Router &other) const = delete;

}; // class Router



}  // namespace cmd
}  // namespace mfx



//#include "mfx/cmd/Router.hpp"



#endif   // mfx_cmd_Router_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
