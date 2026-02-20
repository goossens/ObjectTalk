The input and output pins of a circuit can only carry one specific
signal type. Each of these types is color coded and the IDE ensures
wires are only created between compatible types. Below are all the
signal types used by pins:

![screenshot](img/circuit-pins.png#right)

- **Mono Pins** - carry a mono audio signal.
- **Stereo Pins** - carry a stereo audio signal.
- **Control Pins** - carry a control signal with a value between 0.o0 and 1.0 (analogous to control voltages in modular synths).
- **Frequency Pins** - carry a frequency specification a a stream of number in Hertz.
- **MIDI Pins** - carry a stream of MIDI messages.
