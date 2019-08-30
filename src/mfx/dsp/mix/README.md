## mix — basic copy/mix/volume tools

The classes from this namespace are different implementations of the same functions, providing block-based elementary mixing operations and signal housekeeping:

- Copy
- Mix
- Scaling, with a constant or another signal
- Stereo panning of mono signal (spread)
- Cross-fading
- Matrix operations on two signals
- Interleaving and deinterleaving
- Linear operation ax + b
- Etc.

**`Fpu`** and **`Simd`** are the main implementation classes. **`Generic`** and **`Align`** are wrappers selecting the functions from the most suited classes for easy use within a given context.