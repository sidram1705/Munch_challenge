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

### 3.  Zephyr application

**Action Plan**:

I want to make use of a binary semaphore hand over the functionality to the stepper motors 

**Algorithm**:
