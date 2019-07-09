# Digital Signal Processing library

## General organization

- **`ana`**: analysis and feature extraction tools, except envelope followers
- **`ctrl`**: control signal generators and processors (LFOs, envelopes, parameter smoothing…)
- **`dly`**: delay tools
- **`dyn`**: tools to analyse dynamics (envelope followers)
- **`fir`**: Finite Impulse Response filters
- **`iir`**: Infinite Impulse Response filters, s-plane design tools
- **`mix`**: Basic copy/mix/volume tools
- **`nz`**: Noise generators
- **`osc`**: Oscillators
- **`rspl`**: Resampling tools
- **`shape`**: Waveshapers and look-up table functions
- **`spec`**: Spectral tools
- **`wnd`**: Windows and similar shapes

## Dependencies

The **`dsp`** lib depends on several other libraries:
- **`ffft`**: Fast Fourier Transforms
- **`fstb`**: Miscellaneous support functions and generic tools
- **`hiir`**: half-band IIR filters and Hilbert transforms

## Unsorted classes

- `BandSplitAllPassPair`: A template agregator to split a signal into two bands, using the sum and difference of a complentary pair of all-pass filters


- `FilterCascade`: A template to cascade up to 4 filters from different classes. They rely on a `float process_sample(float)` function.
- `FilterBypass`: A do-nothing filter
- `FilterCascadeIdEven`: A template to cascade several filters from the same class
- `FilterCascadeIdOdd`: Same as `FilterCascadeIdEven`, but with one more filter from another class


- `SplDataRetrievalInterface`: An interface to abstract the random access to potentially large sample data

- `StereoLevel`: a simple 2×2 matrix to handle 2-channel levels
