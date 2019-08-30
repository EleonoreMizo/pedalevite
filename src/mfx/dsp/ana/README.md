## dsp::ana

- **`Freq*`**: Fundamental frequency detectors. Some classes were implementation tests and were not successfull. Maybe they need refinement to work correctly. The only useful algorithms are `FreqYin` and `FreqPeak`. The latter is suitable for real-time implementation but may be subject to occasional octave errors.
- **`ClipDetect`**: detects if the signal jumps above or below a given theshold.
- **`ValSmooth`**: utility class for smoothing the result of a detector wihout losing too much responsiveness.
