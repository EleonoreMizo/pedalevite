## shape — waveshapers and look-up table functions

- **`FncFiniteAsym`**: Look-up table to interpolate any function with regular spacing
- **`MapSaturate`**: Convenient, reciprocal-based shaping function to saturate or desaturate a positive signal. Curvature is a template parameter. Great for remapping the range of a parameter for presentation to the user.
- **`MapSaturateBipolar`**: Same as `MapSaturate`, but symmetric.
- **`Poly3L1Bias`**: Simple and fast asymmetric shaper and clipping function. Intended to be used within the feedback loop of a filter.
- **`WsInterFtorAsym*`**: asymmetric, bounded, monotonic, C1 continuous shaping functors. Unitary slope at (0, 0). [Graphs](https://www.desmos.com/calculator/vdcy3npihw)