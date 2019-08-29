## dsp::dly — delay tools

- **`BbdLine`**: Simple emulation of an ideal Bucket Brigade Device line. Ideal for emulating stompbox modulation effects.
- **`BbdLineReadable`**: same as above, but implements `DelayLineReadInterface`.
- **`DelayLine`**: classic digital delay line implementing `DelayLineReadInterface`.
- **`DelayLineData`**: template class for delay storage and addressing (masking, loop unrolling…)
- **`DelayLineFracFir4Base`**: fractional delay line using a 4-tap FIR as interpolator.
- **`DelayLineReader`**: Class for reading a delay line accessed with a `DelayLineReadInterface`. Handles slow and fast delay time changes, with interpolation and crossfade, respectively.
- **`DelayLineReaderPitch`**: Same as above, with optional basic pitch shifting.
- **`DelayLineReadInterface`**: An interface for reading a delay line, with random access and resampling, and maximum length estimation for block processing (needed when the line is fed back).
- **`DelaySimple`**: a basic and efficient delay class. Does not handle smooth time changes, fractional delay nor resampling.
- **`RingBufVectorizer`**: A utility class to handle several indexes within a ring buffer and compute the maximum block processing length before a cursor wraps back.
