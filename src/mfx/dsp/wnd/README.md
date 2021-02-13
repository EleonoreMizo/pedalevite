## wnd — windowing functions and similar shapes

- **`WndInterface`**: A generic interface to build a window of a given length.
- **`Generic`**: a class implenting `WndInterface` and taking a functor to get the window formula.
- **`XFadeShape`**: a simple template taking an `XFade*` operator and rendering it into a buffer to perform crossfades.
- **`XFadeEqAmp*`**: stateless operators for equal-amplitude crossfading
- **`XFadeEqPow*`**: stateless operators for equal-power crossfading

The other classes are functors for various window functions.
