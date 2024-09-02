#include <Wire.h>
#include <RTClib.h>
#include <JC_Sunrise.h>
#include <LiquidCrystal_I2C.h>
#include <Timezone.h>
#include <L298N.h> // Library for motor control using L298N Dual H bridge
#include <TimeLib.h> // Library for time functions

// I2C bus address for the LCD display
int lcdColumns = 16;
int lcdRows = 2;
int lcdAddress = 0x27; // The display address may vary, check the documentation

LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows);

// Definition of pins for the motor
const int enablePin = 6; // Pin for motor enable (ENA)
const int in1Pin = 7;    // Pin for motor rotation direction (IN1)
const int in2Pin = 8;    // Pin for motor rotation direction (IN2)

// Definition of pins for limit switches
const int switch1Pin = 9;  // Pin for the 1st limit switch
const int switch2Pin = 10; // Pin for the 2nd limit switch

// Definition of location (LAT and LON)
float LAT = 50.993; // Latitude of your location
float LON = 1.689; // Longitude of your location

byte switch1PinS;
byte switch2PinS;

RTC_DS1307 rtc; // Initialization of the RTC module
JC_Sunrise jcSunrise(LAT, LON, JC_Sunrise::officialZenith);

TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};   // Daylight saving time in Central Europe
TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};      // Standard time in the Central Europe
Timezone localTimeZone(CEST, CET);

// Initialization of the object for motor control using L298N
L298N motor(enablePin, in1Pin, in2Pin);

// Constants for motor rotation direction
const int CLOCKWISE = 1;
const int COUNTER_CLOCKWISE = 2;

// Definition of a variable for the time offset for opening and closing the door (in minutes)
const int delayForDoorOpening = 0;  // Time offset for opening the door (0 min = immediately after sunrise)
const int delayForDoorClosing = 45; // Time offset for closing the door (45 min after sunset)

void setup() {
  Serial.begin(9600); // Initialization of the serial connection
  
  // Initialization of the LCD display
  lcd.init();
  lcd.backlight();
  
  // Setting the pins for the motor as output
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

  // Setting the pins for the limit switches as input with an internal pull-up resistor
  pinMode(switch1Pin, INPUT_PULLUP);
  pinMode(switch2Pin, INPUT_PULLUP);

  // Initialization of RTC and setting the time zone to CET
  if (!rtc.begin()) {
    Serial.println("Cannot find RTC");
    while (1);
  }
  
  // Check if the RTC is already running
  if (!rtc.isrunning()) {
    // RTC is not running, set the time to the compilation time
    Serial.println("RTC is not running, setting time to compilation time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC is already running, no need to set the time.");
  }
  
  // Initial time setting based on compilation time, then it can be commented out and the check above uncommented.
  //Serial.println("RTC is not running, setting time to compilation time...");
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // Set the RTC time to a specific date and time (e.g., September 1, 2024, 14:30:00)
  //rtc.adjust(DateTime(2024, 9, 1, 6, 10, 45));
  
  Serial.println("Setup complete.");
  
}

void loop() {
  DateTime now = rtc.now(); // Get the time in UTC
  Serial.println("RTC loaded");
  time_t utc = now.unixtime();
  time_t localTime = localTimeZone.toLocal(utc); // Convert to local time
  
  int sunrise, sunset;
  int utcOffset = 60; // UTC offset for CET (in minutes)
  if (localTimeZone.locIsDST(localTime)) {
    utcOffset = 120; // UTC offset for CEST (in minutes)
  }
  jcSunrise.calculate(localTime, utcOffset, sunrise, sunset);
  
  // Convert Sunrise time to the number of minutes since midnight. Sunrise time is in HHMM format
  int sunrise_hour = sunrise / 100; // Hours
  int sunrise_minutes = sunrise % 100; // Minutes
  int sunrisemidnight = sunrise_hour * 60 + sunrise_minutes;

  // Convert Sunset time to the number of minutes since midnight. Sunset time is in HHMM format
  int sunset_hour = sunset / 100; // Hours
  int sunset_minutes = sunset % 100; // Minutes
  int sunsetmidnight = sunset_hour * 60 + sunset_minutes;
  
  int currentTime = now.hour() * 60 + now.minute();

  // Output to the Serial connection 
  Serial.print("Current time: ");
  print2DigitsSerial(now.hour());
  Serial.print(":");
  print2DigitsSerial(now.minute());
  Serial.println();
  Serial.print("Sunrise: ");
  print2DigitsSerial(sunrise_hour);
  Serial.print(":");
  print2DigitsSerial(sunrise_minutes);
  Serial.print("  Sunset: ");
  print2DigitsSerial(sunset_hour);
  Serial.print(":");
  print2DigitsSerial(sunset_minutes);
  Serial.println();
  Serial.print("Sunset incl. 45 min delay: ");
  print2DigitsSerial((sunsetmidnight + delayForDoorClosing) / 60);
  Serial.print(":");
  print2DigitsSerial((sunsetmidnight + delayForDoorClosing) % 60);
  Serial.println();
  Serial.print("current time: ");
  Serial.println(currentTime);
  Serial.print("sunsetmidnight: ");
  Serial.println(sunsetmidnight);
  Serial.print("sunrisemidnight: ");
  Serial.println(sunrisemidnight);

  // Display information on the LCD display
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(now.day());
  lcd.print(".");
  lcd.print(now.month());
  lcd.print(".");
  lcd.print(now.year());
  lcd.print(" ");
  print2Digits(now.hour());
  lcd.print(":");
  print2Digits(now.minute());
  //lcd.print(":");
  //print2Digits(now.second());

  lcd.setCursor(0, 1);
  print2Digits(sunrise / 100);
  lcd.print(":");
  print2Digits(sunrise % 100);
  lcd.print("  ");
  print2Digits((sunsetmidnight + delayForDoorClosing) / 60);
  lcd.print(":");
  print2Digits((sunsetmidnight + delayForDoorClosing) % 60);
   
  // Open the door at sunrise + time offset
  if (currentTime >= sunrisemidnight + delayForDoorOpening && currentTime < sunsetmidnight) {
    openDoor(); // Function to open the door
  }

  // Close the door at sunset + time offset
  if (currentTime >= sunsetmidnight + delayForDoorClosing || currentTime < sunrisemidnight) {
    closeDoor(); // Function to close the door
  }
  
  //delay(60000); // Wait 1 minute before the next time reading
  delay(1000);  // Wait 1 second
}

void openDoor() {
  // Set the motor rotation direction for opening the door
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  
  // Turn on the motor with a PWM signal
  analogWrite(enablePin, 255); // Maximum motor speed

  // Wait until the upper limit switch is reached (door open)
  while (digitalRead(switch1Pin) == HIGH) {
    delay(100); // Short delay to check the switch state
  }

  // Stop the motor once the upper limit switch is reached
  analogWrite(enablePin, 0); // Stop the motor
}

void closeDoor() {
  // Set the motor rotation direction for closing the door
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  ino
  // Turn on the motor with a PWM signal
  analogWrite(enablePin, 255); // Maximum motor speed

  // Wait until the lower limit switch is reached (door closed)
  while (digitalRead(switch2Pin) == HIGH) {
    delay(100); // Short delay to check the switch state
  }

  // Stop the motor once the lower limit switch is reached
  analogWrite(enablePin, 0); // Stop the motor
}

void print2Digits(int number) {
  if (number < 10) {
    lcd.print("0");
  }
  lcd.print(number);
}

void print2DigitsSerial(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}

void printDateTime(DateTime dateTime) {
  Serial.print(dateTime.year(), DEC);
  Serial.print('/');
  Serial.print(dateTime.month(), DEC);
  Serial.print('/');
  Serial.print(dateTime.day(), DEC);
  Serial.print(" ");
  Serial.print(dateTime.hour(), DEC);
  Serial.print(':');
  Serial.print(dateTime.minute(), DEC);
  Serial.print(':');
  Serial.print(dateTime.second(), DEC);
  Serial.println();
}
