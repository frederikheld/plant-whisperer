// Pins:
const int PIN_CO2_IN = 4;
const int CO2_SENSOR_RANGE = 5000;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_CO2_IN, INPUT);

}

void loop() {

  //int start = millis();
  unsigned long CO2PulseLength = pulseIn(PIN_CO2_IN, HIGH, 2000000);
  //int end = millis();
  //Serial.println(end - start);
  float CO2Percent = (CO2PulseLength / 1000) / 1004.0;

  int CO2PPM = CO2Percent * CO2_SENSOR_RANGE;

  Serial.println(CO2PPM);
  Serial.println(CO2Percent);
  Serial.println("---");

  delay(1000);

}
