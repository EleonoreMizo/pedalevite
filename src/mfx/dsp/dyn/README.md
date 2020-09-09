## dsp::dyn — dynamics analysis

- **`Compander`**: Emulation of a matched compressor/expander, often used along with BBD lines in effect stompboxes to increase their poor SNR.
- **`EnvFollowerAHR1LrSimdHelper`**: Low-ripple and fast response single-channel peak detection (attack/hold/release). 4 sub-followers are running in parallel, with their hold state resetting in a round-robbin fashion periodically. (1)
- **`EnvFollowerAHR4SimdHelper`**: processes 4 envelope detectors (attack/hold/release) on 4 channels simultaneously. (1)
- **`EnvFollowerAR4SimdHelper`**: Processes 4 envelope detectors (attack/release) on 4 channels simultaneously. (1)
- **`EnvFollowerARHelper`**: Envelope detector (attack/release) (1)
- **`EnvFollowerPeak`**: Simple attack/release peak follower
- **`EnvFollowerRms`**: Simple attack/release RMS follower
- **`EnvFollowerRmsSimple`**: RMS follower with a single time for attack and release.
- **`EnvHelper`**: functions to compute envelope time parameters.
- **`LimiterRms`**: hard limiter using RMS envelope detection
- **`MeterRmsPeakHold`**: a utility class to compute both the peak and RMS values of a signal.
- **`MeterRmsPeakHold4Simd`**: same as above, for 4 channels simultaneously.
- **`SCPower`**: Optimised class to prepare a single envelope detection on multi-channel data.

(1) These helpers take only positive values as input. Signals must have been rectified (peak) or squared (RMS) before. Also, the order of the smoothing filter can be selected with a template parameter.
