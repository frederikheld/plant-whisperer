/* CONFIG */

// Pins:
const int PIN_MIC_IN = A0;
const int PIN_CO2_IN = 4;

// CO2 sensor:
const int CO2_SENSOR_RANGE = 5000; // measuring range of the MHZ19 family CO2 sensor --> see data sheet!
const float CO2_SENSOR_PULSE_SCALING_FACTOR = 1004.0; // scaling factor for CO2 sensor readings --> see data sheet!
const int CO2_PPM_THRESHOLD = 3000; // which CO2 PPM value is considered as enough for the plant to be happy?
const int CO2_PPM_BUFFER_SIZE = 5;

// Microphone:
const int MIC_LOUDNESS_THRESHOLD = 1; // above this threshold, microphone readings are considered as "someone is speaking"
const int MIC_READINGS_BUFFER_SIZE = 10;
const int MIC_LOUDNESS_BUFFER_SIZE = 30;

// Scheduler timings (in milliseconds):
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
int co2PPMBuffer[CO2_PPM_BUFFER_SIZE];
int co2PPMBufferIndex = 0;

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
  float co2Percent = (co2PulseLength / 1000) / CO2_SENSOR_PULSE_SCALING_FACTOR;
  int co2PPM = co2Percent * CO2_SENSOR_RANGE;
  
  int average = bufferCo2PPM(co2PPM);

  float slope =  (float) co2PPM / (float) average; // if > 0: value is rising, if < 0: value is falling
  // CONTINUE HERE! How do I store this without making it a global? --> functions need refactoring!

  /*
  // debug slope detection:
  Serial.print("co2PPM:");
  Serial.print(co2PPM);
  Serial.print(",co2PPMAverage:");
  Serial.print(average);
  Serial.print(",slope:");
  Serial.print(slope);
  Serial.println();  
  */
  
  return average;
}

int bufferCo2PPM(int co2PPM) {
  
  // write value to buffer at current index:
  co2PPMBuffer[co2PPMBufferIndex] = co2PPM;

  /*
  // debug buffer contents:
  Serial.print("co2PPMBuffer: [ ");
  for (int i = 0; i < CO2_PPM_BUFFER_SIZE; i++) {
    if (i == co2PPMBufferIndex) Serial.print(">");
    Serial.print(co2PPMBuffer[i]);
    if (i == co2PPMBufferIndex) Serial.print("<");
    Serial.print(" ");
  }
  Serial.println("]");
  */
  
  // move index (circular buffer: if index is out of buffer size, start at 0):
  if (co2PPMBufferIndex >= CO2_PPM_BUFFER_SIZE - 1) {
    co2PPMBufferIndex = 0;
  } else {
    co2PPMBufferIndex++;
  }

  // calculate average over buffer:
  long int sum = 0;
  for (int i = 0; i < CO2_PPM_BUFFER_SIZE; i++) {
    sum += co2PPMBuffer[i];
  }

  int average = sum / CO2_PPM_BUFFER_SIZE;

  /*
  // to debug CO2 buffering:
  Serial.print("co2Sum:");
  Serial.print(sum);
  Serial.print(",co2Average:");
  Serial.print(average);
  Serial.println();
  */

  return average;
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
  long int sum = 0;
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
  long int sum = 0;
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
