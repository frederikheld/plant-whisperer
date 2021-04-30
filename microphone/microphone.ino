

// Thresholds:
const int MIC_ZERO = 827; // der Wert, den das Mic bei Stille ausgibt --> Nullpunkt des Diagramms
const int MIC_LOUDNESS_THRESHOLD = 5; // mic sensitivity
const int MIC_TIME_THRESHOLD = 500; // wie lang in der Vergangenheit gilt es noch als gesprochen

// Pins:
const int PIN_MIC_IN = A0;

// Globals:
int timeMillisCurrent = 0;
int timeMillisLastHeard = millis();

bool isSomeoneSpeaking() {
  int micValue = analogRead(PIN_MIC_IN);
  //Serial.println(micValue);

  //Serial.println(micValue);
  if (abs(micValue - MIC_ZERO) > MIC_LOUDNESS_THRESHOLD) {
    //Serial.println(abs(micValue - MIC_ZERO));
    timeMillisLastHeard = millis();
    return true;
  } else if (timeMillisLastHeard > millis() - MIC_TIME_THRESHOLD) {
    //Serial.println("lastHeard");
    return true;
  }
  
  return false;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_MIC_IN, INPUT);
}

void loop() {

  int timeMillisCurrent = millis();

  bool someoneIsSpeaking = isSomeoneSpeaking();
  Serial.println(someoneIsSpeaking);
    
  //delay(1);

}
