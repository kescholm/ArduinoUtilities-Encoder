# ArduinoUtilities-Encoder

A small library for handling counting and scaling encoders or other sensors for Arduino. The encoding facility (value to counts) can be used for generating pulses to send to a stepper motor, for example. The decoding (counts to value) receives counts from a sensor and handles wrapping when count exceeds bit depth of sensor or crosses 0. If you are using an incremental quadrature encoder, it is recommended to use the excellent [Arduino Encoder library](https://github.com/PaulStoffregen/Encoder) to get number of counts. Then use the counts as input to the `ard_encoder_decode` function in this library.

## Prerequisites

Install [Platform.IO](https://docs.platformio.org/en/latest/) for your IDE. If you want to use the command line, append `"$HOME/.platformio/penv/bin"` to your `PATH` environment variable as instructed by [platformio documentation](https://docs.platformio.org/en/latest/core/installation.html#install-shell-commands).

## Use the Library with Platform.IO or Arduino

In your project, simply add this library in your platform.ini file:

```ini
; Library dependencies
lib_deps =
    ArduinoUtilities-Encoder
```

For more information about libraries for Platform.IO, read up on the [Library Dependency Finder](https://docs.platformio.org/en/latest/librarymanager/ldf.html#ldf)

### Example Usage

First set the encoder parameters:

```cpp
#include <stdint.h>
#include "ArdEncoder.h"
// encoder object
ArdEncoder encoder;
// bit depth of counter (if unsure, use maximum 32)
uint8_t counter_resolution_bits = 32;
// counts per revolution (note the "revolution" does not have to be rotary, it can reference any quantized sensor range)
uint32_t counts_per_rev = 2000;
// value per revolution (this can be in any unit of value, mm, degrees, etc.)
const double value_per_rev = 360.0;
// set parameters
int result = ard_encoder_set(&encoder, counter_resolution_bits, counts_per_rev, value_per_rev);
if (result != 0) {
    // Error setting parameters
}
// reset count and value to zero
ard_encoder_reset(&encoder, 0, 0.0);
```

To get a value from an incremental encoder, at regular intervals in your control loop call the `ard_encoder_decode` function:

```cpp
// get measured value for this tick
int32_t counts = some_sensor_get_counts(...);
// run decode to get sensor value
double value = ard_encoder_decode(&encoder, counts);
```

To generate a number of counts to send to a stepper motor, for example, at regular intervals in your control loop call the `ard_encoder_encode` function:

```cpp
// get new desired command position for next tick
double value = some_trajectory_generator(...);
// run encode to get number of pulses to queue
int32_t pulses_to_send = ard_encoder_encode(&encoder, value);
// Use change in counts to send to stepper.
// Stepper implementation should send pulse train according to step size and number of pulses
queue_pulses_to_stepper(pulses_to_send, ...);
```

You can always access number of counts and value from the `ArdEncoder` object:

```cpp
// current count
int32_t count = encoder.count;
// current value in scaled units
double value = encoder.value;
```

## Setup for Development

If using VS Code, you can easily [install recommended extensions](https://code.visualstudio.com/docs/editor/extension-gallery#_recommended-extensions) for this project.

### Run Examples

Examples are useful when developing the library code. See example code in [examples](./examples/README) folder. Copy and paste source files from example directory to `src` folder then build using Platform.IO. For example:

```sh
cp -r examples/examples/ArdEncoder-CountToValue/* src/
pio run -e atmega328
```

Do not commit any example files in the `src` directory. There must not be any `main` function in the source files for an Arduino library.

### Format Source Files

Source formatting uses LLVM's [Clang Format](https://clang.llvm.org/docs/ClangFormat.html) and the configuration is provided in [`.clang-format`](.clang-format). The configuration is based on default Google style (generated by clang-format version 10.0.0) with `IndentWidth` set to 4 and `ColumnLimit` set to 100.

### Unit Testing

Please read up on Platform.IO unit testing here: [https://docs.platformio.org/en/latest/plus/unit-testing.html](https://docs.platformio.org/en/latest/plus/unit-testing.html). Unit tests are in the `test` folder.

For native unit test, simply run

```sh
pio test -e native
```

For embedded testing with hardware connected by serial ports, set proper upload ports and optionally modify platform.ini with different boards. For example:

```sh
pio test -e esp32 --upload-port /dev/ttyUSB0
pio test -e atmega328 --upload-port /dev/ttyUSB1
```
