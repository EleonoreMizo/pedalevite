## dsp::fir — Finite Impulse Response filters

- **`ConvolverFft`**: Helper class to perform a filtering convolution using an overlap-save-FFT technique.
- **`MovingSum`**: Computes the running sum of the last N samples. Does not drift and works in O(1).
- **`RankSelL`**: Selects the Nth-rank element (the median for example) from a running N-element list. Optimised to select a single, fixed rank per processed sample. Works in O(N)
- **`RankSelA`**: Same as above, but optimised to select one or more ranks very quickly. However new sample insertion is slower than `RankSelL`.
- **`Remez`**: Implementation of the Parks-McClellan algorithm to design a FIR filter.
- **`SlidingOp`**: Performs an operation between the last N samples. The operation is a functor.
