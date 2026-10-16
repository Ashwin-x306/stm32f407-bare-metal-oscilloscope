STM32F407 Bare-Metal Oscilloscope

A single-channel digital oscilloscope built on an STM32F407 using hand-written, register-level peripheral drivers (no ST HAL/LL) and an ST7789 SPI TFT display for waveform rendering.

The MCU samples an analog input via ADC1, computes Vmax/Vmin and the effective sample rate (measured with the DWT cycle counter), and plots the waveform on a grid — with live control over timebase (ms/div), vertical scale (V/div), and vertical position (shift) via push buttons and interrupts. A single button (PA9) switches the display between the time-domain waveform and its frequency spectrum, computed on-chip with a radix-2 decimation-in-time (DIT) FFT.

Features
Real-time waveform capture & display on a 240×320 ST7789 TFT via SPI2
FFT spectrum view using a radix-2 decimation-in-time FFT (4096 points), toggled with a push button (PA9)
Automatic peak-frequency readout from the FFT (strongest bin, DC excluded)
Automatic Vmax / Vmin measurement from captured ADC samples, displayed in volts
Frequency measurement using threshold-crossing detection on sampled ADC data
Adjustable timebase (ms/div) using GPIO external interrupts (EXTI) — in FFT mode the same buttons select Hz/div
Adjustable vertical scale from 10 mV/div to 5 V/div using a configurable lookup table (also scales the FFT amplitude)
Adjustable vertical position of the waveform / spectrum using GPIO external interrupts
Non-blocking button debouncing using the Cortex-M4 DWT cycle counter
Custom register-level drivers for GPIO, SPI, ADC, and timers, developed from scratch without STM32 HAL/LL
Hardware
Component	Details
MCU	STM32F407 (Cortex-M4F, 168 MHz, FPU enabled)
Display	ST7789 TFT, 240×320, driven over SPI2
Signal input	ADC1, Channel 1 (PA1), 12-bit
System clock	168 MHz (SystemClock_Config_168MHz())
Pin Mapping
Pin	Function	Notes
PA0	Reset / re-trigger capture	EXTI0, pull-up/down: none
PA1	Analog signal input	ADC1_IN1
PA2	Shift trace down	EXTI2, pull-down
PA3	Shift trace up	EXTI3, pull-down
PA4	Timebase increase (slower) / Hz/div increase	EXTI4, pull-down
PA5	Timebase decrease (faster) / Hz/div decrease	EXTI9_5, pull-down
PA7	Volts/div decrease	EXTI9_5, pull-down
PA8	Volts/div increase	EXTI9_5, pull-down
PA9	Toggle time domain ⇄ FFT spectrum	EXTI9_5, pull-down
PB13	SPI2_SCK → LCD	AF5
PB14	SPI2_MISO → LCD	AF5
PB15	SPI2_MOSI → LCD	AF5
PD0	LCD Chip Select (CS)	GPIO output
PD1	LCD Data/Command (DC)	GPIO output
PD2	LCD Reset	GPIO output
PD3	LCD Backlight (BL)	GPIO output

Update this table if your actual wiring differs — pull it from your schematic to keep it authoritative.

Results
<img src="1Khz%20PWM.jpeg" alt="1Khz PWM Waveform" width="450">

1 kHz PWM signal captured and rendered on the ST7789 display, showing Vmax/Vmin, timebase, and V/div readout.

<img src="1Khz%20Sine%20Wave.jpg" alt="1Khz Sine Wave" width="450">

1 kHz sine wave capture — waveform trace with grid overlay for timebase and voltage reference.

<img src="FFT%20of%2010%20Khz%20square%20wave.jpg" alt="FFT of 10 Khz square wave" width="450">

FFT of a 10 kHz square wave — frequency spectrum computed on-chip with the radix-2 DIT FFT, shown with the Hz/div scale and peak-frequency readout.

FFT — Radix-2 Decimation-in-Time

The spectrum view is computed entirely on the MCU (no DSP library) by FFT_Compute() in oscilloscope.c, using an in-place radix-2 decimation-in-time (DIT) Cooley–Tukey FFT.

Algorithm

A DFT of size N (a power of two) is split into two DFTs of size N/2 — one over the even-indexed samples and one over the odd-indexed samples:

X[k]       = E[k] + W_N^k · O[k]
X[k + N/2] = E[k] − W_N^k · O[k]          W_N^k = e^(−j·2πk/N),  k = 0 … N/2 − 1

This split is applied recursively until size-2 transforms remain, which are the butterflies. Doing it iteratively gives log2(N) stages of N/2 butterflies each, so the cost drops from O(N²) for a direct DFT to O(N·log2 N). For N = 4096 that is 12 stages × 2048 butterflies.

Processing steps
Take N = 4096 samples (FFT_N) from the start of the 50000-sample capture. The FFT needs a power-of-two length, so the rest of the buffer is used only for the time-domain view and the Vmax/Vmin/frequency measurements.
Convert to volts and remove DC — the mean is subtracted so the 0 Hz bin does not swamp the spectrum.
Apply a Hann window to reduce spectral leakage from the finite capture.
Bit-reversal permutation — the input is reordered so the DIT butterflies can run in place.
Butterfly stages — for each stage (len = 2, 4, … N) the twiddle factor is advanced by a complex rotation (cosf/sinf once per stage, then multiplied incrementally), which avoids a large twiddle table in RAM.
Magnitude — |X[k]| = sqrt(re² + im²) for bins 1 … N/2 − 1 (the upper half is the mirror image for real input).
Peak detection — the strongest bin (DC excluded) gives the peak frequency.
Scaling & axes
Quantity	Formula
Bin spacing	Δf = fs / N (fs = measured effective sample rate)
Peak frequency	f_peak = bin · fs / N
Amplitude (volts)	`A = 4·
Pixels per bin	40 px / (Hz-per-div / Δf) — same "gap" idea as the time-domain plot

On screen the frequency axis runs down the display (0 Hz at the top) and the amplitude runs across, mirroring how the time-domain trace is drawn. The existing buttons keep working in FFT mode:

PA4 / PA5 — Hz/div, stepping through 100 Hz … 100 kHz per division
PA7 / PA8 — V/div (amplitude scale)
PA2 / PA3 — shift the spectrum sideways

The FFT is computed once per capture, the first time FFT mode is shown, and is then reused while you change the scale or shift.

Memory
Buffer	Size
sample_value[]	50000 × 2 B = 100 KB
fft_re[], fft_im[]	2 × 4096 × 4 B = 32 KB
Repository Structure
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
Building & Flashing

This project uses CMake with an ARM cross-compilation toolchain (arm-none-eabi-gcc). The FFT uses sinf, cosf and sqrtf, so link with the math library (-lm) and keep the FPU enabled (-mfpu=fpv4-sp-d16 -mfloat-abi=hard).

bash
# Configure (uses CMakePresets.json)
cmake --preset <preset-name>

# Build
cmake --build build

# Flash (ST-Link)
st-flash write build/stm32f4x-bare-oscilloscope.bin 0x08000000

# or with OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/stm32f4x-bare-oscilloscope.elf verify reset exit"

Replace <preset-name> with whatever preset is defined in your CMakePresets.json (e.g. Debug), and update the output binary name if it differs from what your CMakeLists.txt produces.

This project also includes a .project file, so it can alternatively be imported directly into STM32CubeIDE if you prefer an IDE-based workflow.

How It Works (brief)
Init — clocks, GPIO, SPI2 (LCD), ADC1 (PA1), and EXTI interrupts for all buttons are configured in main().
Capture — SAMPLES (50000) ADC conversions are taken while the DWT cycle counter measures elapsed time, giving an effective sample rate.
Analysis — Vmax/Vmin are found by scanning the buffer and converted to volts (3.3V reference, 12-bit ADC).
Render loop — grid + waveform are drawn to the ST7789 over SPI. Button presses (handled in EXTI*_IRQHandler) update shift, counter (timebase / Hz per div), and volt_div_index (vertical scale) without leaving the render loop, so the display updates live.
Time ⇄ frequency toggle — pressing PA9 flips fft_mode. In FFT mode the radix-2 DIT FFT is run once on the captured data and the spectrum is drawn instead of the waveform.
Pressing the reset button (PA0, EXTI0) sets found = ENABLE, which breaks out of the render loop and triggers a fresh capture.
Known Limitations / TODO
Sampling loop is blocking (ADC_StartConversion + ADC_ReadValue in a tight loop) — no DMA yet, so effective sample rate is limited by software overhead.
The FFT covers the highest frequency up to fs / 2 (Nyquist); the effective sample rate is roughly 100 kHz, so content above ~50 kHz (including higher harmonics of fast square waves) will alias back into the spectrum.
The FFT uses only the first 4096 of the 50000 captured samples, so frequency resolution is fs / 4096 (≈ 24 Hz at 100 kHz).
EXTI9_5_IRQHandler handles four different buttons (PA5, PA7, PA8, PA9) sharing one interrupt line — confirm priority conflicts are acceptable (GPIO_IRQPriorityConfig is called several times for IRQ_NO_EXTI9_5 with different priorities, which will just overwrite the same priority register).
No trigger mode (rising/falling edge trigger) — capture is free-running.
PA9 is also the USB VBUS pin on some STM32F4 Discovery boards — avoid driving it while USB is connected.
FFT peak readout has no interpolation, so the reported frequency is quantised to the bin spacing.
Future Enhancements

Two-channel capture Currently the scope samples a single ADC channel (PA1 / ADC1_IN1). Extending to two channels would involve:

Enabling a second ADC input (e.g. ADC1_IN2, though PA2/PA3 are currently used for buttons — would need re-mapping) and either alternating conversions between channels in the polling loop, or using ADC1 + ADC2 in dual simultaneous mode (supported in hardware on the STM32F4) for true simultaneous sampling without a per-channel rate penalty
A second color-coded trace on the LCD with its own Vmax/Vmin readout UI/button logic to select which channel's V/div and shift are being adjusted

Higher input range (up to ~20 V) The ADC only accepts 0–3.3 V, so this requires external analog conditioning before the pin, not a firmware change alone:

A resistive voltage divider or op-amp attenuator stage to bring 20 V down to ≤3.3 V, ideally switchable (like a real scope's 1x/10x probe setting) so both small and large signals can be read without losing resolution
A corresponding scale factor per range, applied when converting ADC counts back to volts (currently hardcoded as * 3.3f / 4095.0f)

FFT improvements: DMA-driven sampling at a higher, fixed sample rate; FFT size selection; parabolic peak interpolation for sub-bin frequency accuracy; a log (dB) amplitude axis.
