/* CONFIG */

// CO2 sensor:
const int CO2_SENSOR_RANGE = 5000; // Messbereich des Sensors (steht im Datenblatt)
const int CO2_PPM_THRESHOLD = 3000; // ab welchem PPM-Wert ist die Pflanze stark genug "beatmet"

// Microphone:
const int MIC_LOUDNESS_THRESHOLD = 1; // mic sensitivity
const int MIC_TIME_THRESHOLD = 1000; // wie lang in der Vergangenheit gilt es noch als gesprochen
const int MIC_READINGS_BUFFER_SIZE = 10; // über wie viele Messwerte wird gemittelt? Wird bei > 30 instabil!
const int MIC_LOUDNESS_BUFFER_SIZE = 30;

// Pins:
const int PIN_MIC_IN = A0;
const int PIN_CO2_IN = 4;

// Scheduler timings:
const int CO2_READING_INTERVAL = 2000;
const int MIC_READING_INTERVAL = 10;


/* GLOBALS */

// Scheduler execution times:
int schedulerCo2LastExecutionTime = millis();
int schedulerMicLastExecutionTime = millis();

// Buffers:
int micReadingsBuffer[MIC_READINGS_BUFFER_SIZE];
int micReadingsBufferIndex = 0;
int micLoudnessBuffer[MIC_LOUDNESS_BUFFER_SIZE];
int micLoudnessBufferIndex = 0;

// Triggers:
bool co2LevelIsReached = false;
bool someoneIsSpeaking = false;

// Progress:
int progress = 0;


/* FUNCTION DEFINITIONS */

/**
 * Returns the current CO2 value in PPM.
 */
int getCo2PPMValue() {
  unsigned long co2PulseLength = pulseIn(PIN_CO2_IN, HIGH, 2500000);
  float co2Percent = (co2PulseLength / 1000) / 1004.0;
  int co2PPM = co2Percent * CO2_SENSOR_RANGE;

  return co2PPM;
}

/**
 * Returns true if the CO2 level is above the
 * configured CO2_PPM_THRESHOLD.
 */
bool isCo2LevelReached() {
  if (getCo2PPMValue() > CO2_PPM_THRESHOLD) {
    return true;
  }

  return false;
}

/**
 * Returns the rolling average of the given micValue
 * to filter out noise.
 */
int bufferMicReadings(int micValue) {
  
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
 * Returns the rolling average of the given micLoudness
 * to filter out noise.
 */
int bufferMicLoudness(int micLoudness) {
  
  // write value to buffer at current index:
  micLoudnessBuffer[micLoudnessBufferIndex] = micLoudness;

  // move index (circular buffer: if index is out of buffer size, start at 0):
  if (micLoudnessBufferIndex >= MIC_LOUDNESS_BUFFER_SIZE - 1) {
    micLoudnessBufferIndex = 0;
  } else {
    micLoudnessBufferIndex++;
  }

  // calculate average over buffer:
  int sum = 0;
  for (int i = 0; i < MIC_LOUDNESS_BUFFER_SIZE; i++) {
    sum += micLoudnessBuffer[i];
  }

  int average = sum / MIC_LOUDNESS_BUFFER_SIZE;

  return average;
}

/**
 * Returns the microphone loudness which is
 *   * compensated for the drift in the microphone hardware
 *   * buffered to filter out noise
 */
int getMicLoudness() {
  const int micValueRaw = analogRead(PIN_MIC_IN);
  int readingsAverage = bufferMicReadings(micValueRaw);

  /*
  // to debug readings buffering:
  Serial.print("micRaw:");
  Serial.print(micValueRaw);
  Serial.print(",");
  Serial.print("micFiltered:");
  Serial.print(readingsAverage);
  Serial.println();
  */  
  
  int micLoudness = abs(readingsAverage - micValueRaw);
  int micLoudnessAverage = bufferMicLoudness(micLoudness);

  /*
  // to debug loudness buffering:
  Serial.print("loudness:");
  Serial.print(micLoudness);
  Serial.print(",");
  Serial.print("loudnessBuffered:");
  Serial.print(micLoudnessAverage);
  Serial.println();
  */

  return micLoudnessAverage;
}

/**
 * Returns true if the mic loudness is above the
 * configured MIC_LOUDNESS_THRESHOLD.
 */
bool isSomeoneSpeaking() {
  if (getMicLoudness() > MIC_LOUDNESS_THRESHOLD) {
    return true;
  }

  return false;
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
  
  // initialize mic loudness buffer:
  for (int i = 0; i < MIC_LOUDNESS_BUFFER_SIZE; i++) {
    micLoudnessBuffer[i] = 0;
  }
  micLoudnessBufferIndex = 0;

}

void loop() {

  // check if CO2 should be read:
  int currentTime = millis();
  if ((currentTime - schedulerCo2LastExecutionTime) > CO2_READING_INTERVAL) {
    co2LevelIsReached = isCo2LevelReached();
    
    schedulerCo2LastExecutionTime = millis();
  }

  // check if mic should be read:
  if ((currentTime - schedulerMicLastExecutionTime) > MIC_READING_INTERVAL) {
    someoneIsSpeaking = isSomeoneSpeaking();

    schedulerMicLastExecutionTime = millis();
  }

  // update progress:
  if (co2LevelIsReached && someoneIsSpeaking) {
    progress++;
  }

  /*
  // to debug progress:
  Serial.print("progress:");
  Serial.print(progress);
  Serial.println();
  */
  
  
  delay(1);
}
