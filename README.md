# STM32F407 Bare-Metal Oscilloscope

![MCU](https://img.shields.io/badge/MCU-STM32F407-03234B?logo=stmicroelectronics&logoColor=white)
![Core](https://img.shields.io/badge/Core-Cortex--M4F%20%40%20168%20MHz-blue)
![Drivers](https://img.shields.io/badge/Drivers-Bare--metal%20(no%20HAL%2FLL)-success)
![Build](https://img.shields.io/badge/Build-CMake%20%2B%20arm--none--eabi--gcc-orange)

A single-channel digital oscilloscope built on an **STM32F407** using hand-written, register-level peripheral drivers (no ST HAL/LL) and an **ST7789** SPI TFT display for waveform rendering.

The MCU samples an analog input through ADC1, measures Vmax/Vmin and the effective sample rate (using the DWT cycle counter), and plots the waveform on a grid. Timebase, vertical scale and trace position are controlled live with push buttons and interrupts.

A single button (**PA9**) switches the display between the **time-domain waveform** and its **frequency spectrum**, computed on-chip with a **radix-2 decimation-in-time (DIT) FFT**.

---

## Table of Contents

- [Features](#features)
- [Results](#results)
- [Hardware](#hardware)
- [FFT: Radix-2 Decimation-in-Time](#fft-radix-2-decimation-in-time)
- [Repository Structure](#repository-structure)
- [Building & Flashing](#building--flashing)
- [How It Works](#how-it-works)
- [Known Limitations / TODO](#known-limitations--todo)
- [Future Enhancements](#future-enhancements)

---

## Features

| Area | Feature |
|------|---------|
| **Display** | Real-time waveform capture and display on a 240×320 ST7789 TFT over SPI2 |
| **Spectrum** | 4096-point radix-2 DIT FFT, toggled with a push button (`PA9`) |
| **Spectrum** | Automatic peak-frequency readout (strongest bin, DC excluded) |
| **Measurements** | Automatic Vmax / Vmin from captured ADC samples, shown in volts |
| **Measurements** | Frequency measurement by threshold-crossing detection |
| **Controls** | Adjustable timebase (µs/div); in FFT mode the same buttons set **Hz/div** |
| **Controls** | Vertical scale from 10 mV/div to 5 V/div via a lookup table (also scales FFT amplitude) |
| **Controls** | Adjustable trace position (shift) in both views |
| **Firmware** | Non-blocking button debouncing using the Cortex-M4 DWT cycle counter |
| **Firmware** | Custom register-level GPIO, SPI, ADC and timer drivers, written from scratch |

---

## Results

<p align="center">
  <img src="1Khz%20PWM.jpeg" alt="1Khz PWM Waveform" width="450"><br>
  <em>1 kHz PWM signal on the ST7789 display, with Vmax/Vmin, timebase and V/div readout.</em>
</p>

<p align="center">
  <img src="1Khz%20Sine%20Wave.jpg" alt="1Khz Sine Wave" width="450"><br>
  <em>1 kHz sine wave capture, with the grid overlay for timebase and voltage reference.</em>
</p>

<p align="center">
  <img src="FFT%20of%2010%20Khz%20square%20wave.jpg" alt="FFT of 10 Khz square wave" width="450"><br>
  <em>FFT of a 10 kHz square wave, computed on-chip with the radix-2 DIT FFT, shown with the Hz/div scale and peak-frequency readout.</em>
</p>

---

## Hardware

| Component | Details |
|-----------|---------|
| MCU | STM32F407 (Cortex-M4F, 168 MHz, FPU enabled) |
| Display | ST7789 TFT, 240×320, driven over SPI2 |
| Signal input | ADC1, channel 1 (`PA1`), 12-bit |
| System clock | 168 MHz (`SystemClock_Config_168MHz()`) |

### Pin Mapping

| Pin | Function | Notes |
|-----|----------|-------|
| PA0 | Reset / re-trigger capture | `EXTI0`, no pull |
| PA1 | Analog signal input | `ADC1_IN1` |
| PA2 | Shift trace down | `EXTI2`, pull-down |
| PA3 | Shift trace up | `EXTI3`, pull-down |
| PA4 | Timebase increase (slower) / Hz/div increase | `EXTI4`, pull-down |
| PA5 | Timebase decrease (faster) / Hz/div decrease | `EXTI9_5`, pull-down |
| PA7 | Volts/div decrease | `EXTI9_5`, pull-down |
| PA8 | Volts/div increase | `EXTI9_5`, pull-down |
| PA9 | Toggle time domain ⇄ FFT spectrum | `EXTI9_5`, pull-down |
| PB13 | SPI2_SCK → LCD | AF5 |
| PB14 | SPI2_MISO → LCD | AF5 |
| PB15 | SPI2_MOSI → LCD | AF5 |
| PD0 | LCD chip select (CS) | GPIO output |
| PD1 | LCD data/command (DC) | GPIO output |
| PD2 | LCD reset | GPIO output |
| PD3 | LCD backlight (BL) | GPIO output |

> **Note:** update this table if your wiring differs from the schematic.

---

## FFT: Radix-2 Decimation-in-Time

The spectrum view is computed entirely on the MCU (no DSP library) by `FFT_Compute()` in `oscilloscope.c`, using an in-place **radix-2 decimation-in-time (DIT)** Cooley–Tukey FFT.

### Algorithm

A DFT of size *N* (a power of two) is split into two DFTs of size *N/2*, one over the even-indexed samples and one over the odd-indexed samples:

```text
X[k]       = E[k] + W(N,k) · O[k]
X[k + N/2] = E[k] − W(N,k) · O[k]

W(N,k) = e^(−j·2πk/N),   k = 0 … N/2 − 1
```

The split is applied recursively until size-2 transforms remain; these are the **butterflies**. Done iteratively, this gives `log2(N)` stages of `N/2` butterflies each, so the cost falls from O(N²) for a direct DFT to **O(N·log₂N)**. For N = 4096 that is **12 stages × 2048 butterflies**.

### Processing steps

1. **Take N = 4096 samples** (`FFT_N`) from the start of the 50000-sample capture. The FFT needs a power-of-two length, so the rest of the buffer is used only for the time-domain view and the Vmax/Vmin/frequency measurements.
2. **Convert to volts and remove DC.** The mean is subtracted so the 0 Hz bin does not swamp the spectrum.
3. **Apply a Hann window** to reduce spectral leakage from the finite capture.
4. **Bit-reversal permutation.** The input is reordered so the DIT butterflies can run in place.
5. **Butterfly stages.** For each stage (`len` = 2, 4, … N) the twiddle factor is advanced by a complex rotation (`cosf`/`sinf` once per stage, then multiplied incrementally), which avoids a large twiddle table in RAM.
6. **Magnitude.** `sqrt(re² + im²)` for bins `1 … N/2 − 1` (the upper half mirrors the lower half for real input).
7. **Peak detection.** The strongest bin (DC excluded) gives the peak frequency.

### Scaling and axes

| Quantity | Formula |
|----------|---------|
| Bin spacing | `Δf = fs / N` (`fs` = measured effective sample rate) |
| Peak frequency | `f_peak = bin × fs / N` |
| Amplitude (volts) | `A = 4 × magnitude / N` (corrects the Hann window's 0.5 coherent gain) |
| Pixels per bin | `40 px / (Hz-per-div / Δf)`, the same "gap" idea as the time-domain plot |

On screen, the **frequency axis runs down the display (0 Hz at the top)** and the **amplitude runs across**, mirroring how the time-domain trace is drawn. The existing buttons keep working in FFT mode:

| Buttons | Action in FFT mode |
|---------|--------------------|
| `PA4` / `PA5` | Hz/div, stepping from 100 Hz to 100 kHz per division |
| `PA7` / `PA8` | V/div (amplitude scale) |
| `PA2` / `PA3` | Shift the spectrum sideways |

The FFT is computed once per capture, the first time FFT mode is shown, and is reused while you change the scale or shift.

### Memory

| Buffer | Size |
|--------|------|
| `sample_value[]` | 50000 × 2 B = 100 KB |
| `fft_re[]`, `fft_im[]` | 2 × 4096 × 4 B = 32 KB |

---

## Repository Structure

```text
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
├── cmake/                         # CMake toolchain / build config
├── CMakeLists.txt
├── CMakePresets.json
├── stm32f407xg_flash.ld           # Linker script
├── project-description.json
├── .gitignore
└── README.md
```

---

## Building & Flashing

This project uses **CMake** with the ARM cross-compiler (`arm-none-eabi-gcc`). The FFT uses `sinf`, `cosf` and `sqrtf`, so link the math library (`-lm`) and keep the FPU enabled (`-mfpu=fpv4-sp-d16 -mfloat-abi=hard`).

```bash
# Configure (uses CMakePresets.json)
cmake --preset <preset-name>

# Build
cmake --build build

# Flash with ST-Link
st-flash write build/stm32f4x-bare-oscilloscope.bin 0x08000000

# ...or flash with OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/stm32f4x-bare-oscilloscope.elf verify reset exit"
```

> Replace `<preset-name>` with a preset from your `CMakePresets.json` (e.g. `Debug`), and adjust the output filenames if your `CMakeLists.txt` produces different ones.

The repository also includes a `.project` file, so it can be imported directly into **STM32CubeIDE**.

---

## How It Works

1. **Init:** clocks, GPIO, SPI2 (LCD), ADC1 (`PA1`) and the EXTI interrupts for all buttons are configured in `main()`.
2. **Capture:** `SAMPLES` (50000) ADC conversions are taken while the DWT cycle counter measures the elapsed time, giving the effective sample rate.
3. **Analysis:** Vmax/Vmin are found by scanning the buffer and converted to volts (3.3 V reference, 12-bit ADC).
4. **Render loop:** the grid and waveform are drawn to the ST7789 over SPI. Button presses (handled in `EXTI*_IRQHandler`) update `shift`, `counter` (timebase / Hz per div) and `volt_div_index` (vertical scale) without leaving the render loop, so the display updates live.
5. **Time ⇄ frequency toggle:** pressing `PA9` flips `fft_mode`. In FFT mode the radix-2 DIT FFT runs once on the captured data and the spectrum is drawn instead of the waveform.
6. **Re-capture:** pressing the reset button (`PA0`, `EXTI0`) sets `found = ENABLE`, which breaks out of the render loop and starts a fresh capture.

---

## Known Limitations / TODO

- **Blocking sampling loop.** `ADC_StartConversion` + `ADC_ReadValue` run in a tight loop with no DMA, so the effective sample rate is limited by software overhead.
- **Nyquist limit.** The spectrum extends to `fs / 2`. With an effective sample rate of roughly 100 kHz, content above about 50 kHz (including higher harmonics of fast square waves) aliases back into the spectrum.
- **FFT resolution.** Only the first 4096 of the 50000 captured samples are used, so the resolution is `fs / 4096` (about 24 Hz at 100 kHz).
- **Shared EXTI line.** `EXTI9_5_IRQHandler` serves four buttons (PA5, PA7, PA8, PA9). `GPIO_IRQPriorityConfig` is called several times for `IRQ_NO_EXTI9_5` with different priorities, which just overwrites the same priority register, so confirm this is acceptable.
- **No trigger mode.** There is no rising/falling-edge trigger; capture is free-running.
- **PA9 on Discovery boards.** PA9 is also the USB VBUS pin on some STM32F4 Discovery boards, so avoid driving it while USB is connected.
- **No peak interpolation.** The reported peak frequency is quantised to the bin spacing.

---

## Future Enhancements

### Two-channel capture

The scope currently samples a single channel (`PA1` / `ADC1_IN1`). Extending to two channels would involve:

- Enabling a second ADC input (e.g. `ADC1_IN2`; PA2/PA3 are currently used for buttons, so they would need re-mapping) and either alternating conversions between channels in the polling loop, or using ADC1 + ADC2 in dual simultaneous mode (supported in hardware on the STM32F4) for true simultaneous sampling without a per-channel rate penalty.
- A second colour-coded trace on the LCD with its own Vmax/Vmin readout.
- UI/button logic to choose which channel's V/div and shift are being adjusted.

### Higher input range (up to ~20 V)

The ADC only accepts 0–3.3 V, so this needs external analog conditioning, not just a firmware change:

- A resistive divider or op-amp attenuator stage to bring 20 V down to ≤3.3 V, ideally switchable (like a real scope's 1x/10x probe setting) so both small and large signals can be read without losing resolution.
- A matching scale factor per range, applied when converting ADC counts back to volts (currently hard-coded as `* 3.3f / 4095.0f`).

### FFT improvements

- DMA-driven sampling at a higher, fixed sample rate
- Selectable FFT size
- Parabolic peak interpolation for sub-bin frequency accuracy
- A log (dB) amplitude axis
