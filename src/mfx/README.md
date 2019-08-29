# Multi-FX

This is the main Pédale Vite source code.

- **`adrv`**: various audio drivers coded for Pédale Vite and its Windows emulator
- **`cmd`**: the layer commanding the audio thread.
- **`doc`**: the internal document structure, with serialization facilities.
- **`dsp`**: a library containing all the digital signal processing parts.
- **`pi`**: a set of audio effects (and more) used in the pedalboard, designed as plug-ins.
- **`piapi`**: the plug-in API for the audio effects.
- **`ui`**: user interface basics. Mostly deals with hardware (display, switches…)
- **`uitk`**: User interface toolkit. Higher level (G)UI management.

The code uses (or tries to use) a Model-View-Controller (MVC) programming scheme. The **`Model`** and **`View`** classes are the main culprits here. The controller part is split into the various `ui::pg` classes. **`PageSet`** aggregates various GUI parts. **`WorldAudio`** handles most of the real-time audio tasks, and is asynchroniously controlled by `cmd::Central`.

The whole setup and event loops are currently located in `main.cpp` from the topmost source code directory.
