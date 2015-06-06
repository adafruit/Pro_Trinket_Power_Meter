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

// Configure how long before switching mode (in seconds)
#define MODE_SWITCH  5


// Create OLED and INA219 globals.
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_INA219 ina219;

// Keep track of total time and milliamp measurements for milliamp-hour computation.
uint32_t total_sec = 0;
float total_mA = 0.0;

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
  
  // Compute load voltage, power, and milliamp-hours.
  float loadvoltage = busvoltage + (shuntvoltage / 1000);  
  float power_mW = loadvoltage * current_mA;
  total_mA += current_mA;
  total_sec += 1;
  float total_mAH = total_mA / 3600.0;  
  
  // Update display.
  display.clearDisplay();
  display.setCursor(0,0);
  int mode = (total_sec / MODE_SWITCH) % 2;
  if (mode == 0) {
    // Mode 0, display volts and amps.
    printSIValue(loadvoltage, "V:", 2, 10);
    display.setCursor(0, 16);
    printSIValue(current_mA/1000.0, "A:", 5, 10);
  }
  else {
    // Mode 1, display watts and milliamp-hours.
    printSIValue(power_mW/1000.0, "W:", 5, 10);
    display.setCursor(0, 16);
    printSIValue(total_mAH/1000.0, "Ah:", 5, 10);
  }
  display.display();
  
  // Delay for a second.
  delay(1000);
}

void printSIValue(float value, char* units, int precision, int maxWidth) {
  // Print a value in SI units with the units left justified and value right justified.
  // Will switch to milli prefix if value is below 1.
  
  // Add milli prefix if low value.
  if (fabs(value) < 1.0) {
    display.print('m');
    maxWidth -= 1;
    value *= 1000.0;
    precision = max(0, precision-3);
  }
  
  // Print units.
  display.print(units);
  maxWidth -= strlen(units);
  
  // Leave room for negative sign if value is negative.
  if (value < 0.0) {
    maxWidth -= 1;
  }
  
  // Find how many digits are in value.
  int digits = ceil(log10(fabs(value)));
  if (fabs(value) < 1.0) {
    digits = 1; // Leave room for 0 when value is below 0.
  }
  
  // Handle if not enough width to display value, just print dashes.
  if (digits > maxWidth) {
    // Fill width with dashes (and add extra dash for negative values);
    for (int i=0; i < maxWidth; ++i) {
      display.print('-');
    }
    if (value < 0.0) {
      display.print('-');
    }
    return;
  }
  
  // Compute actual precision for printed value based on space left after
  // printing digits and decimal point.  Clamp within 0 to desired precision.
  int actualPrecision = constrain(maxWidth-digits-1, 0, precision);
  
  // Compute how much padding to add to right justify.
  int padding = maxWidth-digits-1-actualPrecision;
  for (int i=0; i < padding; ++i) {
    display.print(' ');
  }
  
  // Finally, print the value!
  display.print(value, actualPrecision);
}

