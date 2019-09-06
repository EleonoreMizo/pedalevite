## osc — oscillators

- **`OscSample`**: Oscillator using a mip-mapped sample data (`SampleData`) of any length.
- **`OscSampleSyncFade`**: Oscillator based on `OscSample`, with soft sync (micro-fade between phase jumps)
- **`OscSampleSyncHard`**: Oscillator based on `OscSample`, with hard sync (straight phase jumps)
- **`OscSinCosEulerSimd`**: Single quadrature sine/cosine oscillator using the Euler algorithm. Requires regular correction to avoid drifting.
- **`OscSinCosStable*`**: Single quadrature sine/cosine oscillator using a more stable algorithm. SIMD and FPU versions available.
- **`OscWavetable`**: Oscillator using a mip-mapped periodic waveform (`WavetableData`)
- **`OscWavetableSub`**: Twin oscillator similar to `OscWavetable`, allowing to subtract two phase-shifted versions of the waveform. For example two saws will produce a rectangle with ajustable pulse width.
- **`OscWavetableSyncHard`**: Antialiased sync’ed oscillator, master part.
- **`SampleData`**: Container for mip-mapped samples.
- **`SampleMipMapper`**: Automatic generation of the SampleData` mip-maps from the source data.
- **`SweepingSin`**: Sweeping sinus generator (log frequency spacing).
- **`WavetableData`**: Storage class for mip-mapped sample tables.
- **`WavetableMipMapper`**: Utility class to build the mip-maps of a WavetableData.
- **`WavetablePolyphaseData`**: Storage class for polyphase filters.
- **`WpdGen`**: Functions to fill a `WavetablePolyphaseData`, for example with a BLEP.

Below are more detailed explanations of some of these classes.

### OscWavetable

This is a basic wavetable oscillator.
It uses octave-spaced mip-mapped data, provided in a `WavetableData` object.
You’ll have to fill the first table with your own waveform, and use `WavetableMipMapper` to automatically fill all the other tables.
Of course, wavetables can be shared between multiple instance of similar oscillators.
The oscillator also needs an interpolator.

For best results, `OscWavetable` should be oversampled.
Indeed, the top octave of the spectrum may not be filled with all the harmonics, depending on the pitch.
It will also contain aliasing caused by the slow rolloff (and first sidelobes) of the interpolator frequency response.
Usually, oversampling it twice is enough.
You can use one of the `hiir::Downsampler2x*` classes for this purpose.

The oscillator is controlled with logarithmic pitch.
Each additional 0x10000 raises the pitch from one octave.
The base pitch, which can be redefined at runtime, corresponds to the Nyquist frequency.
You cannot reach a pitch this high, therefore any pitch value should be less than the base.
If you don’t have any clue about how to compute the pitch, member function `conv_freq_to_pitch()` will help you to find the right value.

You can set the phase manually at any time, however there is no functionality to make it work smoothly.
See `OscWavetableSyncHard` for anti-aliased phase resets.

#### Template parameters

- **`IF`**: the interpolator functor

You’ll find working functors in `dsp::rspl::InterpFtor`.
`CubicHermite` is probably your best bet.
`DATA_PRE` and `DATA_POST` members constants can be used directly to feed `UPRE` and `UPOST` (see below).

- **`MAXSL2`**, **`MINSL2`**, **`OVRL2`**: the main wavetable parameters

`MAXSL2` and `MINSL2` are the log2 of the maximum and minium size of the wavetables.

You generally want to have all the harmonics down to the lowest pitch reachable by the oscillator.
For example, a waveform of 25 Hz at 44.1 kHz, oversampled twice (88.1 kHz) is 3524 sample-long.
So using 2^12 = 4096 samples does the job for `MAXSL2`.

A naive implementation would use critically-sampled mip-maps.
Each mip-map would be half the size of the previous one, containing exactly half the harmonics.
This saves a lot of space, but this does not lead to the best results.
It may be preferable to use pre-oversampled data, especially when the pitch is high and the aliasing becomes more noticeable by folding back significant power into the lower part of the spectrum.

The minimum wavetable size allows the oscillator to use pre-oversampled source data for the highest pitches.
The mip-maps will never be shorter than 2^MINSL2 samples.

In addition, you can pre-oversample your data with `OVRL2`.
Again, pass the log2 of the actual value.
The oversampling is constrained in the bottom pitch range by the maximum table size, which can not be exceeded.
This helps keeping a decent size for the whole wavetable, relaxing the oversampling where the aliasing is less prone to be noticed.

- **`DT`**: sample data type

This is the data type of the wavetables as well as the oscillator output.
Depending on the implementation, integer types may be faster.
Only `float` ant `int16_t` have been tested.

When generating the wavetable with DT as an integer type, make sure that there is enough headroom for the interpolation overshoots.
It is safe to limit the waveform amplitude to half the maximum absolute value of the type.

- **`UPRE`**, **`UPOST`**: unrolling margins

These are wavetable parameters too.
Unrolling margins are extra samples at the both end of each waveform period.
Thus, the oscillator does not have to wrap a pointer during the interpolation of the samples located near the ends.

#### Typical instantiation and use

```
typedef float DataType;
typedef mfx::dsp::rspl::InterpFtor::CubicHermite Interpolator;

typedef mfx::dsp::osc::OscWavetable <
	Interpolator,
	12, 6, 3,
	DataType,
	Interpolator::DATA_PRE, Interpolator::DATA_POST
> OscType;
typedef typename OscType::WavetableDataType WtType;

OscType osc;
WtType  wt;

const int last_table = wt.get_nbr_tables () - 1;
const int table_len  = wt.get_table_len (last_table);
for (int pos = 0; pos < table_len; ++pos)
{
	wt.set_sample (last_table, pos, /*** your computed sample value ***/);
}
mfx::dsp::osc::WavetableMipMapper <WtType> mipmapper;
mipmapper.build_mipmaps (wt);
osc.set_wavetable (wt);

osc.set_pitch (osc.conv_freq_to_pitch (1000, 44100));
osc.process_block (data_ptr, 5000);
```

Not covered here: oversampling.

### OscWavetableSub

This is an oscillator with a “subtraction” option.
Two waveforms with a variable phase shift are subtracted from each other.
For example, with identical saw waveforms, one can obtain a rectangle waveform with PWM (pulse width modulation),
ranging from ultra-thin Dirac pulse to plain square.

The oscillator works more or less the same as `OscWavetable`.
It has two outputs:

- The waveform from the first wavetable, exactly like `OscWavetable`.
- The difference between the first and the second wavetables.

Note that there can be a DC offset, depending on the waveforms and the phase difference.
You can remove it with a simple high-pass filter like `dsp::iir::DcKiller1p`.
Or you can use some explicit computed value with the `dc_fixer` argument when setting the phase difference.
This allows fast response to waveform changes and preserves all the bass frequencies.
If you do the second way for achieving phase difference modulation, be sure to adjust the parameters at the sample rate to avoid zipper noise.

### OscWavetableSyncHard

This is a hard-sync’ed oscillator.
It is the master part of the duo and uses a standard oscillator as slave.
The master resets the phase of the slave oscillator at each cycle master.
The slave is the true signal generator, the master only provides synchronisation information.

The slave oscillator could be an `OscWavetable` or an `OscWavetableSub`.
It is configured via the `OscWavetableSyncHard` interface, but can be directly accessed to setup specific features, for example to set different wavetables for `OscWavetableSub`.

The master requires an antialiased step table (aka BLEP, Band Limited stEP) in order to cancel the aliasing generated by the phase reset.
Only the 0-order discontinuities are healed this way.
Slope breaks and further Nth-order discontinuities would need extra-processing, not featured here.
The BLEP is contained in a `WavetablePolyphaseData` and can be setup with `WpdGen::build_blep_pure()`.
Note that the BLEP causes a slight latency in the oscillator output, half the BLEP phase length.

The basic interface is similar to `OscWavetable`.
The pitch of the slave oscillator can be set independently of the master pitch.
The reference pitch is identical for both oscillators.
It is also possible to set the phase reset position within the slave oscillator waveform which could provide significant tone changes at low slave frequencies.
Like the other oscillators, this one should be oversampled for optimal results.

#### Template parameters

- **`OSC`**: the slave oscillator.

- **`STPPLEN`**: antialiased step length

This is the length of a single BLEP phase.
Currently, the step must occurs exactly at the middle of the phase 0.
Therefore it should be even.
It is also limited to 1024.

- **`STPNPL2`**: Log2 of the number of phases of the BLEP.

- **`STPLVL2`**: Log2 of the step level.

This parameter is useful only for integer data type and should be set to 0 with floating point types.
It indicates the nominal level of the step, so it can be scaled accordingly.
Some headroom is required for the step overshoots.
You generally want to set it at the total bitdepth of the data type minus two.

#### Typical instantiation and use

```
typedef float DataType;
typedef mfx::dsp::rspl::InterpFtor::CubicHermite Interpolator;
typedef mfx::dsp::osc::OscWavetable <
	Interpolator,
	12, 6, 3,
	DataType,
	Interpolator::DATA_PRE, Interpolator::DATA_POST
> OscInternal;
typedef mfx::dsp::osc::OscWavetableSyncHard <
	OscInternal, 16, 8,
	  (std::is_integral <DataType>::value)
	? std::numeric_limits <DataType>::digits - 1
	: 0
> OscType;
typedef typename OscType::WavetableDataType WtType;
typedef typename OscType::AntialiasedStep StepType;

OscType  osc;
WtType   wt;
StepType st;

const int last_table = wt.get_nbr_tables () - 1;
const int table_len  = wt.get_table_len (last_table);
for (int pos = 0; pos < table_len; ++pos)
{
	wt.set_sample (last_table, pos, /*** your computed sample value ***/);
}
mfx::dsp::osc::WavetableMipMapper <WtType> mipmapper;
mipmapper.build_mipmaps (wt);
osc.set_wavetable (wt);

wnd::Generic <double, wnd::CoefGenBHMinLobe> win_gen;
mfx::dsp::osc::WpdGen::build_blep_pure (
	st, win_gen, 1 << OscType::PHASE_LEVEL_L2
);
osc.set_steptable (st);

osc.set_pitch (osc.conv_freq_to_pitch (440, 44100));
osc.set_pitch_slave (osc.conv_freq_to_pitch (1000, 44100));
osc.process_block (data_ptr, 5000);
```

### OscSample

*To do*

### OscSampleSyncHard and OscSampleSyncFade

*To do*
