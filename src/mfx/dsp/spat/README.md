# spat — spatialisation, environment

Building blocks and complete models for reverberation and acousitcs simulation.

- **`ApfLine`**: reverberator building block, made of all-pass delays, a simple delay and a filter
- **`Cascade`**: Utility class to cascade two audio processors
- **`DelayAllPass`**: Simple all-pass delay diffusion block
- **`DelayFrac`**: fractional delay using 4-tap FIR interpolation
- **`EarlyRef`**: Early-refelection processor, using a multi-tap delay
- **`ReverbDattorro`**: implementation of a reverberator described by Jon Dattorro
- **`ReverbSC`**: implementation of a reverberator designed by Sean Costello

Subnamespaces:

- **`fv`**: implementation of the freeverb model
- **`ltc`**: adaptation of the Takamitsu Endo’s LatticeReverb model
