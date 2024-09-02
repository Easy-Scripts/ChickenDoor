# Automatic Door Controller for Chicken Coop
This project is an automatic door controller designed to open and close a door based on sunrise and sunset times. The system uses an RTC (Real-Time Clock) module to keep track of the time and a motor driver to control the door movement. The project is particularly useful for automating the opening and closing of a chicken coop door.

## Features
* **Automatic Door Control:** The door opens automatically at sunrise and closes at sunset with configurable time offsets.
* **Real-Time Clock (RTC):** Keeps accurate time, even when the system is powered off.
* **LCD Display:** Shows current date, time, sunrise, and sunset times.
* **Motor Control:** Uses L298N dual H-bridge motor driver to control the door movement.
* **Limit Switches:** Ensures the door stops moving when fully open or closed.

## Components Used
* **Arduino UNO:** The microcontroller used to run the code.
* **RTC Module (DS1307/DS3231):** Keeps track of the current time.
* **L298N Motor Driver:** Controls the DC motor responsible for opening and closing the door.
* **DC Motor:** Moves the door.
* **Limit Switches:** Detect when the door is fully open or closed.
* **LiquidCrystal_I2C (16x2 LCD Display):** Displays time, date, and other status information.
* **JC_Sunrise Library:** Calculates sunrise and sunset times based on geographical location.
* **Timezone Library:** Handles time zone conversions, including daylight saving time adjustments.
* **Wires and Connectors:** For connecting all the components.

## How It Works
* **RTC Module:** The RTC module provides the current date and time. This is used to calculate the time of sunrise and sunset. The initial time is set when
the code is compiled and uploaded to the Arduino. After setting the correct time during the first upload, you should comment out the time-setting code and
upload the modified version to prevent resetting the time on subsequent uploads/resets or after power outage.
* **JC_Sunrise Library:** Based on your geographical location (latitude and longitude), the JC_Sunrise library calculates the exact times for sunrise and sunset.
* **Motor Control:** The L298N motor driver controls the DC motor. The motor is responsible for opening and closing the door. The direction of the motor is controlled by the Arduino.
* **Limit Switches:** The limit switches ensure that the door does not move beyond its physical limits (fully open or fully closed).
* **LCD Display:** The 16x2 LCD display shows the current time, date, and calculated sunrise and sunset times, so you can monitor the system.

## Libraries Used
* **Wire.h:** For I2C communication with the RTC module.
* **RTClib.h:** For interfacing with the DS1307 RTC module.
* **JC_Sunrise.h:** For calculating sunrise and sunset times.
* **LiquidCrystal_I2C.h:** For controlling the LCD display.
* **Timezone.h:** For handling time zone conversions.
* **L298N.h:** For controlling the motor using the L298N motor driver.
* **TimeLib.h:** For handling various time-related functions.

## Usage
The door will automatically open at sunrise and close at sunset based on the configured time offsets.
The current time, date, and calculated sunrise/sunset times are displayed on the LCD screen.
The system automatically handles daylight saving time based on the configured time zone.

## Customization
* **Time Offsets:** You can modify the *`delayForDoorOpening`* and *`delayForDoorClosing`* constants in the code to change the time offsets for opening and closing the door.
* **Motor Speed:** Adjust the PWM value in the *`analogWrite(enablePin, 255)`*; lines in the *`openDoor`* and *`closeDoor`* functions to control the motor speed.
* **Geographical Location:** To ensure accurate sunrise and sunset calculations, update the *`LAT`* and *`LON`* variables in the code with your specific latitude and longitude coordinates. These values are essential for calculating the correct times for your location.

## License
This project is open-source and available under the MIT License.
