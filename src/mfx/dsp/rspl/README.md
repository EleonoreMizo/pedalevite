## rspl — resampling tools

- **`InterpFirMakerHermite4`**: Utility class for building a 3rd-order cubic hermite interpolator as a 4-tap FIR
- **`InterpFirMakerLagrange4`**: Utility class for building a 3rd-order lagrange interpolator as a 4-tap FIR
- **`InterpolatorInterface`**: generic interpolator interface. Provides sample-per-sample and block processing, resampling rate linear ramping and multi-channel operations.
- **`InterpolatorFir`**: implements the `InterpolatorInterface` using a FIR for interpolation. The FIR convolver class should be provided as a template parameter.
- **`InterpolatorHermite43`**: implements the `InterpolatorInterface` using a 3rd-order cubic hermite interpolator
- **`InterpolatorHermite43Simd`**: same as `InterpolatorHermite43` with SIMD processing
- **`InterpolatorLerp`**: implements the `InterpolatorInterface` using a linear interpolator
- **`InterpolatorLerpSimd`**: same as `InterpolatorLerp` with SIMD processing
- **`InterpolatorOrder0`**: implements the `InterpolatorInterface` using a 0-th order interpolator (nearest neighbour)
- **`SnhTool`**: This class is intended to improve resampling speed in presence of oversampled processing (rendering at a high sampling rate then downsampling to the target sampling rate). The idea is to generate less samples than required (in other words, at a lower rate) and to let the object hold the values to fill the blanks. Sample & Hold is preferred here over zero-filling because it greatly simplifies rate changes.
- **`Voice`**: class build around `InterpolatorInterface` and `SplDataRetrievalInterface`, handling the playback of a sampler voice with variable rate. Optional forward, backward or bidirectional looping.
