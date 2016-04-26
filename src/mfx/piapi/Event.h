
#pragma once

#include "mfx/piapi/EventNoteOn.h"
#include "mfx/piapi/EventNoteOff.h"
#include "mfx/piapi/EventParam.h"
#include "mfx/piapi/EventTransport.h"

namespace mfx
{
namespace piapi
{

union Event
{
	EventNoteOn    _note_on;
	EventNoteOff   _note_off;
	EventParam     _param;
	EventTransport _transport;
};

}	// namespace piapi
}	// namespace mfx
