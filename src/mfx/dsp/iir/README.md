## dsp::iir — Infinite Impulse Response filters, s-plane design tools

- **`AllPass1p`**: single-pole all-pass filter
- **`AllPass1pChain`**: Chain of several single-pole all-pass filters
- **`AllPass2p`**: 2-pole all-pass filter
- **`AllPassCascade`**: Cascade of any number of 1-pole and 2-pole all-pass filter sections
- **`Biquad`**: Direct Form 2 biquad (2 poles and 2 zeros) implementation
- **`Biquad4`**: Processes 4 biquads in parallel. The interface is compatible with `Biquad4Simd`.
- **`Biquad4Simd`**: Processes 4 biquads simultaneously. They can be arranged in parallel, series or both. Serial arrangements can be with or without latency compensation.
- **`Biquad4SimdMorph`**: Same as above, with automatic coefficient ramping when they are changed.
- **`BiquadPackSimd`**: Processes N channels with M biquads, for an almost optimal usage of the SIMD instructions. The (N, M) configuration can be smoothly changed, as well as each filter coefficient. This is the core engine of any complex equalizer.
- **`DcKiller1p`**: single-pole DC removal.
- **`DcKiller2p`**: 2-pole DC removal.
- **`DesignEq2p`**: s-plane design functions for basic filters (butterworth, equalizer…), including Orfanidis’ corrected filters at Nyquist frequency.
- **`Downsampler?xSimd`**, **`Upsampler?xSimd`**: Convenience wrappers on the HIIR functions to achieve fast and high-quality 4x-up and downsampling.
- **`FilterComb`**: Sub-sample comb filter with custom interpolator.
- **`Lpf1p`**: Simple smoothing low-pass filter with one pole and no zero
- **`OnePole`**: Direct Form 2 implementation of 1st order filter.
- **`SplitAp5`**: 5th-order crossover filter using complementary pairs of all-pass filters.
- **`SplitMultiband`**: Filterbank with equal-magnitude reconstruction using complementary pairs of all-pass filters (any order, variable number of bands).
- **`SplitMultibandLin*`**: Filterbank with perfect phase-linear reconstruction using IIR filters.
- **`SplitThiele8`**: Linkwitz-Riley or Thiele 8th-order crossover filter.
- **`SqueezerSimd`**: 4-pole Moog low-pass filter emulation with optional, custom dirt. **`SqueezerOpDefect`** (radical shaping and polarity inversion) can be used for this purpose.
- **`Svf2p`**: class to design SVFs from basic parameters or s-plane equations
- **`SvfCore`**: Like `Biquad`, but using Andrew Simper’s Linear Trapezoidal State Variable Filters (SVFs) instead of biquads. Filter and mixer coefficients can be computed with `Svf2p`. Generic mixer can be optimised with **`SvfMixer*`** classes if only a fixed output is required.
- **`SvfCore4Simd`**: Processes 4 SVFs simultaneously. Works like **`Biquad4Simd`**.
- **`TransS`**: Transforms of s-plane equations and poles/zeros. 
- **`TransSZBilin`**: Functions turning s-plane equations (analogue prototypes) into z-plane coefficients to use them in the filter implementations. Also handles frequency prewarping.
- **`TransSZMatched`**: Vicanek’s transforms of s- to z-plane equations, trying to fix the frequency response near the Nyquist frequency.

