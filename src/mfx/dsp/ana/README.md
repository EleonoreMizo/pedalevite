## dsp::ana

- **`Freq*`**: Fundamental frequency detectors. Some classes were implementation tests and were not successfull. Maybe they need refinement to work correctly. The only useful algorithms are `FreqYin` and `FreqPeak`. The latter is suitable for real-time implementation but may be subject to occasional octave errors.
- **`ClipDetect`**: detects if the signal jumps above or below a given theshold.
- **`ValSmooth`**: utility class for smoothing the result of a detector wihout losing too much responsiveness.
- **`OnsetNinos2`**: NINOS² onset detector. Good results but high latency.

### EBU R 128 meters

**`r128`** sub-namespace

These classes provide building blocks for EBU R 128 compliant meters, as well as ready-to-use meters.

- **`AllMeters`**: Single class containing all the meters: momentary loudness, short-term loudness, integrated loudness, loudness range and true peak.
- **`ChnMerger`**: Building blocks to handle multi-channel content.
- **`FilterK`**: Filter for K-weighting, before detecting the loudness.
- **`HistoLu`**: Building block for integrated loudness and loudness range detection. Can run in a real-time thread (fixed memory usage).
- **`MeanSq`**: Mean square measure with rectangular windowing
- **`PeakDetectStd`**, **`PeakDetectAccurate`**: True peak detectors. The standard one is implemented as specified in ITU-R BS.1770, the accurate one uses quadratic interpolation to get a more accurate estimation of the true peak level.
- **`SegmentHelper`**: Helper to mix block processing with fixed-length analysis frames.
- **`Upsampler`**: Building block implementing 4x or 2x oversampling for true peak detection.
