**jef-sure**

this is esp32-c3 mcu. OutpuEnablePin is pulled up with 10kohm resistor. why after reset during mcu initialisation for short time there's 0 on that pin?

**GitHub Copilot**

This behavior is expected on ESP32-C3 (and most ESP32 chips) due to the default state of GPIOs during reset and boot:

GPIOs are configured as inputs by default after reset.
If you have a 10k pull-up resistor on OutputEnablePin (GPIO0), the pin should be high when floating.
However, ESP32-C3 GPIO0 is a strapping pin:
During reset/boot, the internal pull-down is enabled on GPIO0 to read the boot mode (download/normal).
This internal pull-down is much stronger than your 10k pull-up, so the pin is briefly pulled low during boot.
After boot, when your code configures the pin as output and sets it high (or enables PWM), it will go high as expected.

**Summary:**

The brief low on OutputEnablePin after reset is caused by the ESP32-C3's internal bootstrapping mechanism, which temporarily pulls GPIO0 low to check the boot mode. This overrides your external pull-up until the MCU finishes booting and your firmware configures the pin.

**Reference:**

* ESP32-C3 Technical Reference Manual, Strapping Pins
* ESP32-C3 Datasheet, Section 4.2 Strapping Pins

If you need OutputEnablePin to stay high during reset:

Use a different GPIO that is not a strapping pin (avoid GPIO0, GPIO2, GPIO8, GPIO9, GPIO10).
Or, accept this brief low as a hardware limitation of the ESP32-C3.

**jef-sure**

I changed pin to GPIO3, but it didn't help

**GitHub Copilot**

Changing to GPIO3 does not solve the issue because GPIO3 is also a strapping pin on ESP32-C3.
Specifically, GPIO3 is used for the UART download mode (see ESP32-C3 Strapping Pins Table).

During reset/boot, the ESP32-C3 may briefly override the state of GPIO3 (just like GPIO0), causing it to go low regardless of your pull-up resistor.

**How to fix this?**

Choose a GPIO that is NOT a strapping pin for OutputEnablePin.
On ESP32-C3, avoid using:
GPIO2, GPIO8, GPIO9, GPIO10, GPIO3, and GPIO0 for signals that must remain high during reset/boot.
**Recommended:**

Pick a GPIO that is not listed as a strapping pin (for example, GPIO5, GPIO6, GPIO7, GPIO18, etc., if available on your board).

**Summary:**

The brief low is a hardware limitation of the ESP32-C3 strapping pin mechanism.
Use a non-strapping GPIO for OutputEnablePin to avoid this issue.

**jef-sure**

I changed pin to GPIO5 but it "glitches" several times after reset

**GitHub Copilot**

GPIO5 on ESP32-C3 is not a strapping pin, so it should not be affected by the bootstrapping logic. However, you may still see glitches (brief low or toggling) on GPIO5 after reset due to these reasons:

1. Default State After Reset
All GPIOs are inputs after reset.
With a 10k pull-up, the pin should stay high, but any brief internal leakage or floating state can cause a glitch until your firmware configures it as an output or PWM.
2. LEDC/PWM Initialization
When you configure GPIO5 for PWM (LEDC), there may be a brief moment where the pin is set as output low before the duty cycle is applied.
The ESP-IDF LEDC driver may momentarily drive the pin low during timer/channel setup.
3. Firmware Startup Delay
There is a delay between reset and when your code runs and configures the pin.
During this time, the pin is not actively driven by the MCU.
How to Minimize Glitches
Use a Stronger Pull-up

Try a lower value resistor (e.g., 1kΩ) to keep the pin high more firmly during boot.
Configure GPIO Early

If possible, configure GPIO5 as output high (or set PWM duty to max) as early as possible in your app_main or even in a custom bootloader.
Check LEDC Duty Initialization

Make sure you set the duty cycle to maximum (0xFF for 8-bit) before enabling the channel, if possible.
Board Layout

Ensure there is no capacitive coupling or leakage on the PCB that could cause glitches.

**Summary**

* GPIO5 is not a strapping pin—the glitch is likely due to the default input state and/or the way the LEDC driver initializes the pin.
* A brief glitch is normal on most MCUs before firmware takes control.
* **To minimize:** use a stronger pull-up and configure the pin as output/PWM as early as possible.
If you need the pin to be guaranteed high at all times, you must use external circuitry (like a power-on reset IC or buffer with OE controlled by a delayed supervisor) to mask the MCU's boot period. This is a common embedded hardware challenge.