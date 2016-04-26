
#pragma once

namespace mfx
{
namespace piapi
{

class EventNoteOn
{
public:
	int            _note_id;	// > 0.
	float          _note;      // MIDI pitch
};

}	// namespace piapi
}	// namespace mfx
