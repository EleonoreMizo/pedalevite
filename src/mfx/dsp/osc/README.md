## osc — oscillators

- **`OscSinCosEulerSimd`**: Single quadrature sine/cosine oscillator using the Euler algorithm. Requires regular correction to avoid drifting.
- **`OscSinCosStable*`**: Single quadrature sine/cosine oscillator using a more stable algorithm. SIMD and FPU versions available.
- **`SweepingSin`**: Sweeping sinus generator (log frequency spacing).
- **`WavetableData`**: Storage class for mip-mapped sample tables.
- **`WavetableMipMapper`**: Utility class to build the mip-maps of a WavetableData.
- **`WavetablePolyphaseData`**: Storage class for polyphase filters.
