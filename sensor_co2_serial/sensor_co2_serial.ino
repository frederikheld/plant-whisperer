/**
 * NOTE: this is work in progress and I didn't finish it yet as I found a way to get the PWM approach working without interfering with the mic.
 */

// Includes:
#include "MHZ19.h"
#include <SoftwareSerial.h>

// Pins:
#define PIN_CO2_RX 1
#define PIN_CO2_TX 0
#define CO2_SENSOR_RANGE 5000
#define CO2_SENSOR_SERIAL_BAUD_RATE 112500

// Config:
#define MHZ19_ERRORS false

// Globals:
MHZ19 co2Sensor;
SoftwareSerial co2SensorSerial(PIN_CO2_RX, PIN_CO2_TX);

void setup() {
  // configure hardware serial (via USB):
  Serial.begin(112500);

  // configure sofware serial (via GPIO):
  co2SensorSerial.begin(CO2_SENSOR_SERIAL_BAUD_RATE);

  // configure CO2 sensor:
  co2Sensor.begin(co2SensorSerial);
  co2Sensor.autoCalibration(false);
  // NOTE: auto-calibration is only useful if the sensor stays on for several weeks.
  // See docs: https://github.com/WifWaf/MH-Z19

  


}

void loop() {

/*
  //int start = millis();
  unsigned long CO2PulseLength = pulseIn(PIN_CO2_IN, HIGH, 2000000);
  //int end = millis();
  //Serial.println(end - start);
  float CO2Percent = (CO2PulseLength / 1000) / 1004.0;

  int CO2PPM = CO2Percent * CO2_SENSOR_RANGE;

  Serial.println(CO2PPM);
  Serial.println(CO2Percent);
  Serial.println("---");
*/

  int CO2PPM = co2Sensor.getCO2();
  Serial.println(CO2PPM);

  delay(1000);

}
