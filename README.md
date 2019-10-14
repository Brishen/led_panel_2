# LED Panel
## Components
1 x [Adafruit Metro M0](https://www.adafruit.com/product/3505)
1 x [Adafruit RGB Matrix Shield](https://www.adafruit.com/product/2601)
1 x [16x32 RGB Panel](https://www.adafruit.com/product/420)
1 x [Sparkfun GPS Module](https://www.sparkfun.com/products/retired/11058)
1 x [BME680 Sensor](https://www.adafruit.com/product/3660)
1 x [DS3231 RTC](https://www.adafruit.com/product/3013)

# Connections
The BME680 and DS3231 use I2C

## DS3231
### Power Pins:

-   **Vin**  - this is the power pin. Since the RTC can be powered from 2.3V to 5.5V power, you do not need a regulator or level shifter for 3.3V or 5V logic/power. To power the board, give it the same power as the logic level of your microcontroller - e.g. for a 5V micro like Arduino, use 5V
-   **GND**  - common ground for power and logic

### I2C Logic pins:

-   **SCL** - I2C clock pin, connect to your microcontrollers I2C clock line. This pin has a 10K pullup resistor to Vin
-   **SDA** - I2C data pin, connect to your microcontrollers I2C data line. This pin has a 10K pullup resistor to Vin

### Other Pins:

-   **BAT**  - this is the same connection as the positive pad of the battery. You can use this if you want to power something else from the coin cell, or provide battery backup from a different separate batery. VBat can be between 2.3V and 5.5V and the DS3231 will switch over when main Vin power is lost
-   **32K**  - 32KHz oscillator output. Open drain, you need to attach a pullup to read this signal from a microcontroller pin
-   **SQW**  - optional square wave or interrupt output. Open drain, you need to attach a pullup to read this signal from a microcontroller pin
-   **RST**  - This one is a little different than most RST pins, rather than being just an input, it is designed to be used to reset an external device or indicate when main power is lost. Open drain, but has an internal 50K pullup. The pullup keeps this pin voltage high as long as Vin is present. When Vin drops and the chip switches to battery backup, the pin goes low.

### Assembly
-   Connect  **Vin**  to the power supply, 3-5V is fine. Use the same voltage that the microcontroller logic is based off of. For most Arduinos, that is 5V
-   Connect  **GND** to common power/data ground
-   Connect the  **SCL**  pin to the I2C clock  **SCL**  pin on your Arduino. On an UNO & '328 based Arduino, this is also known as  **A5**, on a Mega it is also known as  **digital 21**  and on a Leonardo/Micro,  **digital 3**
-   Connect the  **SDA**  pin to the I2C data  **SDA** pin on your Arduino. On an UNO & '328 based Arduino, this is also known as  **A4**, on a Mega it is also known as  **digital 20**  and on a Leonardo/Micro,  **digital 2**


### Code
Probably gonna want to use this https://github.com/adafruit/RTClib

## BME680
### Power Pins:

-   **Vin**  - this is the power pin. Since the sensor chip uses 3 VDC, we have included a voltage regulator on board that will take 3-5VDC and safely convert it down. To power the board, give it the same power as the logic level of your microcontroller - e.g. for a 5V micro like Arduino, use 5V
-   **3Vo**  - this is the 3.3V output from the voltage regulator, you can grab up to 100mA from this if you like
-   **GND**  - common ground for power and logic
### SPI Logic pins:
All pins going into the breakout have level shifting circuitry to make them 3-5V logic level safe. Use whatever logic level is on  **Vin!**

-   **SCK**  - This is the  **S**PI  **C**loc**k**  pin, its an input to the chip
-   **SDO**  - this is the  **S**erial  **D**ata  **O**ut /  **M**aster  **I**n  **S**lave  **O**ut pin, for data sent from the BME680 to your processor
-   **SDI**  - this is the  **S**erial  **D**ata  **I**n /  **M**aster  **O**ut  **S**lave  **I**n pin, for data sent from your processor to the BME680
-   **CS**  - this is the  **C**hip  **S**elect pin, drop it low to start an SPI transaction. Its an input to the chip

If you want to connect multiple BME680's to one microcontroller, have them share the SDI, SDO and SCK pins. Then assign each one a unique CS pin.

### I2C Logic pins:

-   **SCK** - this is  _also_ the I2C clock pin, connect to your microcontrollers I2C clock line.
-   **SDI** - this is  _also_  the I2C data pin, connect to your microcontrollers I2C data line.

Leave the other pins disconnected

## Assembly
### I2C Wiring

Use this wiring if you want to connect via I2C interface

By default, the i2c address is 0x77. If you add a jumper from SDO to GND, the address will change to 0x76.

-   [![adafruit_products_metro-i2c_bb.png](https://cdn-learn.adafruit.com/assets/assets/000/048/068/medium640/adafruit_products_metro-i2c_bb.png?1510169836)](https://learn.adafruit.com/assets/48068)

-   Connect  **Vin**  to the power supply, 3-5V is fine. Use the same voltage that the microcontroller logic is based off of. For most Arduinos, that is 5V. For 3.3V logic devices, use 3.3V
-   Connect  **GND** to common power/data ground
-   Connect the  **SCK**  pin to the I2C clock  **SCL**  pin on your Arduino compatible
-   Connect the  **SDI**  pin to the I2C data  **SDA** pin on your Arduino compatible

### SPI Wiring

Since this is a SPI-capable sensor, we can use hardware or 'software' SPI. To make wiring identical on all microcontrollers, we'll begin with 'software' SPI. The following pins should be used:

-   [![adafruit_products_metro-spi_bb.png](https://cdn-learn.adafruit.com/assets/assets/000/048/071/medium640/adafruit_products_metro-spi_bb.png?1510170026)](https://learn.adafruit.com/assets/48071)

-   Connect  **Vin**  to the power supply, 3V or 5V is fine. Use the same voltage that the microcontroller logic is based off of
-   Connect  **GND** to common power/data ground
-   Connect the  **SCK**  pin to  **Digital #13**  but any pin can be used later
-   Connect the  **SDO**  pin to  **Digital #12**  but any pin can be used later
-   Connect the  **SDI**  pin to  **Digital #11**  but any pin can be used later
-   Connect the  **CS**  pin  **Digital #10**  but any pin can be used later

## Code
https://github.com/adafruit/Adafruit_BME680

## RGB Panel
Just connect it up..

### Code
1. https://github.com/adafruit/RGB-matrix-Panel
2. https://github.com/adafruit/Adafruit-GFX-Library



## ATSAMD
[https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports](https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports)
<!--stackedit_data:
eyJoaXN0b3J5IjpbNTM0MjI5ODQ3LDE2NDYzODEzNzhdfQ==
-->