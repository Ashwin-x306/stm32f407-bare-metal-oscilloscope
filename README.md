# STM32F407 Bare-Metal Oscilloscope

A single-channel digital oscilloscope built on an **STM32F407** using hand-written,
register-level peripheral drivers (no ST HAL/LL) and an **ST7789** SPI TFT display
for waveform rendering.

The MCU samples an analog input via ADC1, computes Vmax/Vmin and the effective
sample rate (measured with the DWT cycle counter), and plots the waveform on a
grid — with live control over timebase (ms/div), vertical scale (V/div), and
vertical position (shift) via push buttons and interrupts.

---

## Features

- **Real-time waveform capture & display** on a 240×320 ST7789 LCD over SPI2
- **Auto Vmax / Vmin readout** of the captured buffer, shown in volts
- **Auto Frequency Detection of the sampled ADC values
- **Adjustable timebase** (ms/div) via up/down buttons (`EXTI9_5`)
- **Adjustable vertical scale** (V/div) from a lookup table (10 mV/div → 5 V/div)
  via up/down buttons
- **Adjustable vertical position (shift)** of the trace via up/down buttons
  (`EXTI2` / `EXTI3`)
- **Debounced button inputs** using the DWT cycle counter (no blocking delays)
- **Custom register-level drivers** for GPIO, SPI, and ADC (written from scratch,
  no HAL)

---

## Hardware

| Component        | Details                                   |
|-------------------|-------------------------------------------|
| MCU               | STM32F407 (Cortex-M4, 168 MHz)            |
| Display           | ST7789 TFT, 240×320, driven over SPI2     |
| Signal input      | ADC1, Channel 1 (`PA1`), 12-bit           |
| System clock      | 168 MHz (`SystemClock_Config_168MHz()`)   |

### Pin Mapping

| Pin  | Function                        | Notes                          |
|------|----------------------------------|---------------------------------|
| PA0  | Reset / re-trigger capture       | `EXTI0`, pull-up/down: none    |
| PA1  | Analog signal input              | ADC1_IN1                        |
| PA2  | Shift trace down                 | `EXTI2`, pull-down              |
| PA3  | Shift trace up                   | `EXTI3`, pull-down              |
| PA4  | Timebase increase (slower)       | `EXTI4`, pull-down              |
| PA5  | Timebase decrease (faster)       | `EXTI9_5`, pull-down            |
| PA7  | Volts/div decrease                | `EXTI9_5`, pull-down            |
| PA8  | Volts/div increase                | `EXTI9_5`, pull-down            |
| PB13 | SPI2_SCK  → LCD                  | AF5                              |
| PB14 | SPI2_MISO → LCD                  | AF5                              |
| PB15 | SPI2_MOSI → LCD                  | AF5                              |
| PD0  | LCD Chip Select (CS)             | GPIO output                     |
| PD1  | LCD Data/Command (DC)            | GPIO output                     |
| PD2  | LCD Reset                        | GPIO output                     |
| PD3  | LCD Backlight (BL)               | GPIO output                     |

> Update this table if your actual wiring differs — pull it from your schematic
> to keep it authoritative.

---
## Results

<img src="1Khz%20PWM.jpg" alt="1Khz PWM Waveform" width="450">

*1 kHz PWM signal captured and rendered on the ST7789 display, showing Vmax/Vmin, timebase, and V/div readout.*

<img src="1Khz%20Sine%20Wave.jpg" alt="1Khz Sine Wave" width="450">

*1 kHz sine wave capture — waveform trace with grid overlay for timebase and voltage reference.*
## Repository Structure

```
.
├── drivers/
│   ├── inc/                       # Custom register-level driver headers
│   │   ├── stm32f405xxx.h
│   │   ├── stm32f405xxx_gpio_driver.h
│   │   ├── stm32f405xxx_spi_driver.h
│   │   ├── stm32f405xxx_adc_driver.h
│   │   ├── ST7789.h
│   │   ├── sys_clock.h
│   │   ├── fft.h
│   │   └── image.h
│   └── src/                       # Driver implementations
│       ├── stm32f405xxx_gpio_driver.c
│       ├── stm32f405xxx_spi_driver.c
│       ├── stm32f405xxx_adc_driver.c
│       ├── ST7789.c
│       ├── sys_clock.c
│       └── fft.c
├── Src/
│   ├── oscilloscope.c
│   ├── startup_stm32f407xx.S
│   ├── syscall.c
│   └── sysmem.c
├── cmake/                          # CMake toolchain / build config
├── CMakeLists.txt
├── CMakePresets.json
├── stm32f407xg_flash.ld            # Linker script
├── project-description.json
├── .gitignore
└── README.md
```

---

## Building & Flashing

This project uses **CMake** with an ARM cross-compilation toolchain
(`arm-none-eabi-gcc`).

```bash
# Configure (uses CMakePresets.json)
cmake --preset <preset-name>

# Build
cmake --build build

# Flash (ST-Link)
st-flash write build/stm32f4x-bare-oscilloscope.bin 0x08000000

# or with OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/stm32f4x-bare-oscilloscope.elf verify reset exit"
```

> Replace `<preset-name>` with whatever preset is defined in your
> `CMakePresets.json` (e.g. `Debug`), and update the output binary name if it
> differs from what your `CMakeLists.txt` produces.

This project also includes a `.project` file, so it can alternatively be
imported directly into **STM32CubeIDE** if you prefer an IDE-based workflow.

---

## How It Works (brief)

1. **Init** — clocks, GPIO, SPI2 (LCD), ADC1 (PA1), and EXTI interrupts for all
   buttons are configured in `main()`.
2. **Capture** — `SAMPLES` (50000) ADC conversions are taken back-to-back while
   the DWT cycle counter measures elapsed time, giving an effective sample rate.
3. **Analysis** — Vmax/Vmin are found by scanning the buffer and converted to
   volts (`3.3V` reference, 12-bit ADC).
4. **Render loop** — grid + waveform are drawn to the ST7789 over SPI. Button
   presses (handled in `EXTI*_IRQHandler`) update `shift`, `counter` (timebase),
   and `volt_div_index` (vertical scale) without leaving the render loop, so the
   display updates live.
5. Pressing the reset button (`PA0`, `EXTI0`) sets `found = ENABLE`, which breaks
   out of the render loop and triggers a fresh capture.

---

## Known Limitations / TODO

- Sampling loop is blocking (`ADC_StartConversion` + `ADC_ReadValue` in a tight
  loop) — no DMA yet, so effective sample rate is limited by software overhead.
- `EXTI9_5_IRQHandler` handles three different buttons (PA5, PA7, PA8) sharing
  one interrupt line — confirm priority conflicts are acceptable
  (`GPIO_IRQPriorityConfig` is called three times for `IRQ_NO_EXTI9_5` with
  different priorities, which will just overwrite the same priority register).
- No trigger mode (rising/falling edge trigger) — capture is free-running.
- FFT support is stubbed out (`//#include "fft.h"`) but not implemented.

---
## Future Enhancements

Two-channel capture Currently the scope samples a single ADC channel (PA1 / ADC1_IN1). Extending to two channels would involve:

- Enabling a second ADC input (e.g. ADC1_IN2, though PA2/PA3 are currently used for buttons — would need re-mapping) and either alternating conversions between channels in the polling loop, or using ADC1 + ADC2 in dual simultaneous mode (supported in hardware on the STM32F4) for true simultaneous sampling without a per-channel rate penalty
- A second color-coded trace on the LCD with its own Vmax/Vmin readout
UI/button logic to select which channel's V/div and shift are being adjusted

Higher input range (up to ~20 V) The ADC only accepts 0–3.3 V, so this requires external analog conditioning before the pin, not a firmware change alone:

- A resistive voltage divider or op-amp attenuator stage to bring 20 V down to ≤3.3 V, ideally switchable (like a real scope's 1x/10x probe setting) so both small and large signals can be read without losing resolution
- A corresponding scale factor per range, applied when converting ADC counts back to volts (currently hardcoded as * 3.3f / 4095.0f)
- Input protection (clamping diodes) so voltage above 3.3 V can't damage the MCU pin if the divider/protection circuit fails

## License
MIT LICENSE
