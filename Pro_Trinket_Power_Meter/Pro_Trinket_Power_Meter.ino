// Pro Trinket Power Meter
//
// Small Pro Trinket-based power monitor using an INA219 breakout and 
// monochrome OLED display.
//
// Author: Tony DiCola
//
// See the full guide at:
//   https://learn.adafruit.com/pro-trinket-power-meter/overview
//
// Released under a MIT license: http://opensource.org/licenses/MIT
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>


// Configure the pin connected OLED reset/RST.
#define OLED_RESET   4

// Configure orientation of the display.
// 0 = none, 1 = 90 degrees clockwise, 2 = 180 degrees, 3 = 270 degrees CW
#define ROTATION     2


// Create OLED and INA219 globals.
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_INA219 ina219;

void setup()   {
  // Setup the INA219.
  ina219.begin();
  // By default the INA219 will be calibrated with a range of 32V, 2A.
  // However uncomment one of the below to change the range.  A smaller
  // range can't measure as large of values but will measure with slightly
  // better precision.
  //ina219.setCalibration_32V_1A();
  //ina219.setCalibration_16V_400mA();
  
  // Setup the OLED display.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Clear the display.
  display.clearDisplay();
  display.display();
  
  // Set rotation.
  display.setRotation(ROTATION);

  // Set text size and color.
  display.setTextSize(2);
  display.setTextColor(WHITE);
}

void loop() {
  // Read voltage and current from INA219.
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  // Update display.
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(" V: "); display.println(loadvoltage, 1);
  display.print("mA: "); display.print(current_mA, 1);
  display.display();
  
  // Delay for a second.
  delay(1000); 
}

