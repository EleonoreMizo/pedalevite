
#pragma once

#include "mfx/piapi/Event.h"
#include "mfx/piapi/EventType.h"

namespace mfx
{
namespace piapi
{

class EventTs
{
public:
	int            _timestamp; // Samples, relative to the block beginning, >= 0
	EventType      _type;
	Event          _evt;
};

}	// namespace piapi
}	// namespace mfx
