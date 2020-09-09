## va — virtual analogue circuit simulations

These classes are audio processors modelled from electronic circuits

- **`Antisat*`**: Antisaturator functions, to be used in `SvfAntisat`.
- **`DiodeClipDAngelo`**: Simple passive antiparallel diode clipper, using an explicit expression for the non-linearity.
- **`DiodeClipJcm`**: . Diode clipper in the feedback path of an operational amplifier with inverting input. Modelled from the gain stage of a JCM900-4100 Hi Gain Dual Reverb head.
- **`DiodeClipScreamer`**: Diode clipper in the feedback path of an operational amplifier with non-inverting input. Modelled from the Tube Screamer gain stage.
- **`Iv*`**: I(V) and dI/dV(V) functions, to be used in `RcClipGeneric`.
- **`MoogLadderDAngelo`**: Moog filter simulation, using the D’Angelo & Valimaki algorithm
- **`MoogLadderMystran`**: 4-pole Moog-like filter, algorithm by Mystran
- **`RcClipGeneric`**: Simple diode clipper (with RC low-pass filtering), using a generic Newton-Raphson to solve the non-linearity. It is possible to use different non-linearities, not just diode characteristics.
- **`SolverNR`**: Newton-Raphson unidimentional solver
- **`SvfAntisat`**: A State-Variable Filter simulation with antisaturation in the damping path.

Subnamespaces:

- **`dkm`**: circuit simulator using the nodal DK method
- **`mna`**: circuit simulator using the Modified Nodal Analysis (MNA)
