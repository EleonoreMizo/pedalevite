# Digital Signal Processing library

## General organization

- **`ana`**: analysis and feature extraction tools, except envelope followers
- **`ctrl`**: control signal generators and processors (LFOs, envelopes, parameter smoothing…)
- **`dly`**: delay tools
- **`dyn`**: tools to analyse dynamics (envelope followers)
- **`fir`**: Finite Impulse Response filters
- **`grn`**: Granular resynthesis
- **`iir`**: Infinite Impulse Response filters, s-plane design tools
- **`mix`**: Basic copy/mix/volume tools
- **`nz`**: Noise generators
- **`osc`**: Oscillators
- **`rspl`**: Resampling tools
- **`shape`**: Waveshaper, look-up table functions and distortions
- **`spat`**: Spatialisation, environment
- **`spec`**: Spectral tools
- **`va`**: Virtual analogue circuit modelisations
- **`wnd`**: Windows and similar shapes

## Dependencies

The **`dsp`** lib depends on several other libraries:
- **`ffft`**: Fast Fourier Transforms
- **`fstb`**: Miscellaneous support functions and generic tools
- **`hiir`**: Half-band IIR filters and Hilbert transforms
- **`lal`**: Minimalist linear algebra library

## Unsorted classes

- **`BandSplitAllPassPair`**: A template agregator to split a signal into two bands, using the sum and difference of a complentary pair of all-pass filters


- **`DenormStop`**: Functions to prevent denormal numbers to appear and significantly slow down the processing.


- **`FilterCascade`**: A template to cascade up to 4 filters from different classes. They rely on a `float process_sample(float)` function.
- **`FilterBypass`**: A do-nothing filter
- **`FilterCascadeIdEven`**: A template to cascade several filters from the same class
- **`FilterCascadeIdOdd`**: Same as `FilterCascadeIdEven`, but with one more filter from another class


- **`SplDataRetrievalInterface`**: An interface to abstract the random access to potentially large sample data

- **`StereoLevel`**: a simple 2×2 matrix to handle 2-channel levels
