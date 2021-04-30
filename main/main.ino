
// Scheduler timings:
const int CO2_READING_INTERVAL = 1000;
const int MIC_READING_INTERVAL = 100;


// Globals:
int co2ReadingLastExecutionTime = millis();
int micReadingLastExecutionTime = millis();


void setup() {
  Serial.begin(9600);
  Serial.println("setup()");
}

void loop() {

  // check if CO2 should be read:
  int currentTime = millis();
  if (currentTime - co2ReadingLastExecutionTime > CO2_READING_INTERVAL) {
    Serial.println("READ CO2 <=="); // hier führen wir später die Funktion aus
    co2ReadingLastExecutionTime = millis();
  }

  // check if mic should be read:
  //currentTime = millis();
  if (currentTime - micReadingLastExecutionTime > MIC_READING_INTERVAL) {
    Serial.println("READ MIC");
    micReadingLastExecutionTime = millis();
  }
  
  delay(1);

}
