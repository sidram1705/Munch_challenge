# Munch_challenge

## Progress date: 14 Sept

### 1. Breadboard Circuit Assembly

After reviewing the datasheets of the motor driver and stm nucleo board, I noted the power requirements and pins that can be connected according to these requirements. This was the setup implemented by me:

| Pins | Connection | Reasoning |
| --- | --- | ---|
| EN | PA7 (STM32) | Enable pin just needs a digital High/Low, something we need to turn on and off the stepper motor to our exact rotation |
| MS1 | VCCIO (STM32) | The microstep pin connection would determines the multiplier for the angle of rotation. I decided to go with 16X by connecting both the pins to 5V|
| MS2 | VCCIO (STM32) | The microstep pin connection would determines the multiplier for the angle of rotation. I decided to go with 16X by connecting both the pins to 5V |
| Rx | x | Left floating because not using the UART mode of motor driver |
| Tx | x | Left floating because not using the UART mode of motor driver |
| CLK | x | Left floating because clock signals are for legacy motor drivers |
| STEP | PA5 (STM32) | Specifically chose the pin for the timer/counter functionality |
| DIR | PA6 (STM32) | Just a High/Low digital pin for the direction of rotation |
| VM | 12V | From the digital voltage source provided |
| GND | GND | Just grounding with the voltage source |
| A2 | Pin 1 (motor) | Four pins connecting the stepper motor to the motor driver |
| A1 | Pin 2 (motor) | Four pins connecting the stepper motor to the motor driver |
| B1 | Pin 3 (motor) | Four pins connecting the stepper motor to the motor driver |
| B2 | Pin 4 (motor) | Four pins connecting the stepper motor to the motor driver |
| VIO | VCC (STM32) | 5V from the MCU|
| GND | GND | From the MCU |

**NOTE**

A 100uF capacitor was added in series with the battery to act as a reservoir from the digital 12V and a 0.1uF was placed near the VS and GND pin to filter out high frequency noise. 

**Circuit Assembly**:

<div align=center>
<img src="/Images/Circuit_assembly.jpeg" width="600">  
</div>

### 2. Schematic in KiCAD

The TMC2209 driver wasn't readily available from the existing drivers, so I created a symbol for the <a href="Munch_challenge/tmc2209.kicad_sym">motor driver</a>. 

**Motor Driver Symbol**

<div align=center>
<img src="/Images/tmc2209.png" width="600">  
</div>

Using this symbol, I laid out the schematic based on the hardware assembly as mentioned above. The shcematic file is uploaded <a href="Munch_challenge/Munch_stepper_motor.kicad_sch">here</a>

**Schematic**

<div align=center>
<img src="/Images/schematic.png" width="600">  
</div>

### 3.  Working Code

I wasn't able to setup the environment with zephyr to make the code run, so I tested it out first without zephyr using the STM32CubeIDE. The files that I ran the code on are linked in <a href="Munch_challenge/Inc">Inc</a> and <a href="Munch_challenge/Src">Src</a>. In order to make a working system for the stepper motor, I divided my file structure as follows:

- **delay module**: provides accurate microsecond delays using a hardware timer (TIMx).
- **stepper driver**: wraps STEP/DIR/EN control, angle→steps math, and motion primitives.
- **main**: initializes the HAL, clock, GPIO, timer, delay module, and the stepper; then runs your demo motions.

1. Delay:

System: HSI = 16 MHz, APB2 prescaler = DIV1 ⇒ timer_clk = 16 MHz.
TIM1 PSC = 215 (from your .ioc)

> f_cnt = 16,000,000 / (215 + 1) ≈ 74,074.07 Hz
> ticks = 500 × 74,074.07 / 1e6 ≈ 37.0 ticks  → wait until counter reaches ~37

2. Stepper:

- Init sets known-safe pin states and leaves the driver disabled by default.
- Enable/Disable toggles the EN pin respecting polarity.
- SetDir(dir) sets the DIR pin (driver samples DIR on the STEP edge; our delays are huge vs datasheet ns-scale constraints).
- StepN(n) performs n STEP pulses:
    - For each pulse:
        - Drive STEP high → wait step_pulse_us
        - Drive STEP low → wait step_space_us
    - Overall step period (µs):
    > T_step = step_pulse_us + step_space_us
    - Step frequency (Hz):
    > f_step = 1,000,000 / T_step

- AngleToSteps(angle_deg) converts degrees to microsteps using rounding (prevents cumulative error):
> steps = round( angle_deg × (full_steps_per_rev × microsteps) / 360 )

With 200 FSR and 1/16:
- 30° → 200×16×30/360 = 266.67 → 267 steps
- 45° → 200×16×45/360 = 400.00 → 400 steps
- 90° → 800 steps
- 180° → 1600 steps

- RotateAngle(angle_deg, dir) = set DIR → enable → StepN(AngleToSteps(...)) → disable.

3. Main:

    1. HAL & clocks: HAL_Init(), SystemClock_Config() (HSI 16 MHz in your project).
    2. GPIO & TIM1 init: CubeMX-generated MX_GPIO_Init() and MX_TIM1_Init() set:
        - STEP/DIR/EN as push-pull outputs.
        - TIM1 with your PSC (e.g., 215) and an ARR big enough to cover delays.
    3. Delay module: Delay_Init(&htim1, start=1)
        - Reads the actual timer clock + PSC and computes f_cnt for correct µs timing.
    4. Stepper config: Fill the StepperConfig with your pins, 200 steps/rev, 16 microsteps, and 500/500 µs pulse/space Initialize with an initial direction.
    5. Demo loop: Repeatedly call RotateAngle with 30°, 45°, 90°, 180° clockwise, sleeping 1 s between moves.

The working wideo can be viewed in <a href="/Images/Stepper_motor.mp4"></a>