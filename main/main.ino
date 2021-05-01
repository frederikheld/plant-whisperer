// Pins:
const int PIN_MIC_IN = A0;
const int PIN_CO2_IN = 4;

// CO2 config:
const int CO2_SENSOR_RANGE = 5000; // Messbereich des Sensors (steht im Datenblatt)

// Microphone config:
const int MIC_LOUDNESS_THRESHOLD = 10; // mic sensitivity
const int MIC_TIME_THRESHOLD = 1000; // wie lang in der Vergangenheit gilt es noch als gesprochen
const int MIC_READINGS_BUFFER_SIZE = 10; // über wie viele Messwerte wird gemittelt? Wird bei > 30 instabil!

// Scheduler timings:
const int CO2_READING_INTERVAL = 2000;
const int MIC_READING_INTERVAL = 10;

// Globals:
int schedulerCo2LastExecutionTime = millis();
int schedulerMicLastExecutionTime = millis();
int timeMillisLastHeard = millis();

int micReadingsBuffer[MIC_READINGS_BUFFER_SIZE];
int micReadingsBufferIndex = 0;

// Functions:
int getCO2PPMValue() {
  
  unsigned long CO2PulseLength = pulseIn(PIN_CO2_IN, HIGH, 2500000);

  float CO2Percent = (CO2PulseLength / 1000) / 1004.0;

  int CO2PPM = CO2Percent * CO2_SENSOR_RANGE;

  return CO2PPM;
  
}

bool isSomeoneSpeaking() {
  if (getMicLoudnessCompensated() > MIC_LOUDNESS_THRESHOLD) {
    timeMillisLastHeard = millis();
    return true;
  } else if (timeMillisLastHeard > (millis() - MIC_TIME_THRESHOLD)) {
    return true;
  }

  return false;
}

/**
 * This function returns the rolling average of
 * the given micValue to filter out noise.
 */
int bufferMicValue(int micValue) {
  
  // write value to buffer at current index:
  micReadingsBuffer[micReadingsBufferIndex] = micValue;

  // move index (circular buffer: if index is out of buffer size, start at 0):
  if (micReadingsBufferIndex >= MIC_READINGS_BUFFER_SIZE - 1) {
    micReadingsBufferIndex = 0;
  } else {
    micReadingsBufferIndex++;
  }

  // calculate average over buffer:
  int sum = 0;
  for (int i = 0; i < MIC_READINGS_BUFFER_SIZE; i++) {
    sum += micReadingsBuffer[i];
  }

  int average = sum / MIC_READINGS_BUFFER_SIZE;

  return average;
}

/**
 * This function compensates loudness for the drift in the
 * microphone hardware by calculating the loudness compared
 * to the rolling average of microphone readings.
 */
int getMicLoudnessCompensated() {
  const int micValue = analogRead(PIN_MIC_IN);
  int readingsAverage = bufferMicValue(micValue);
  return abs(readingsAverage - micValue); 
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_CO2_IN, INPUT);
  pinMode(PIN_MIC_IN, INPUT);

  // initialize mic readings buffer:
  for (int i = 0; i < MIC_READINGS_BUFFER_SIZE; i++) {
    micReadingsBuffer[i] = 0;
  }
  micReadingsBufferIndex = 0;

}

void loop() {

  // check if CO2 should be read:
  int currentTime = millis();
  if ((currentTime - schedulerCo2LastExecutionTime) > CO2_READING_INTERVAL) {
    //Serial.println("READ CO2 <==");
    //int co2PPM = getCO2PPMValue();
    //Serial.println(co2PPM);
    //Serial.println(10);
    schedulerCo2LastExecutionTime = millis();
  }

  // check if mic should be read:
  if ((currentTime - schedulerMicLastExecutionTime) > MIC_READING_INTERVAL) {
    int micLoudnessCompensated = getMicLoudnessCompensated();
    Serial.println(micLoudnessCompensated);

    schedulerMicLastExecutionTime = millis();
  }
  
  delay(1);
}
