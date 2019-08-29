## dsp::ctrl — control signal generators and processors

### Envelope generator

**`env`** sub-namespace

- **`AdsrRc`**: a class generating analogue-like ADSR envelopes, emulating the load and discharge of RC sections. The sustain time can be fixed or infinite.
- **`SegmentRc`**: building block for envelope generator, emulates a single RC section.

### LFO

**`lfo`** sub-namespace

- **`LfoModule`**: a complete LFO, with a lot of parameters and waveforms
- **`PhaseGen`**: utility class to generate a periodic phase.
- **`PhaseGenChaos`**: Chaotic phase modulator, keeps track of the overall phase and period of the main phase generator so everything stays globally in sync.
- **`Osc*`**: classes generating a single waveform, building blocks for `LfoModule`.

### Other classes

- **`InertiaLin`**: a linear parameter smoother, the target value is reached in constant time.
- **`Ramp`**: similar to `InertiaLin`, but specialized for length-variable block processing. You can set the length of the block and the value to reach at the end. It keeps track of the values at the beginning and the end of the block, so they can be fed directly to a block interpolation function.