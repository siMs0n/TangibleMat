int irAnalogPin = A0;
int restAreaButtonPin = 0;
int coffeeButtonPin = 1;
int musicButtonPin = 2;
int lightButtonPin = 3;
int nightLightButtonPin = 4;

int nightLightOutputPin = 5;
int lightstripClockPin = 6;
int lightstripOutputPin = 7;

struct RawInput{
  boolean musicButton;
};

struct ProcessedInput {
  boolean musicButtonPressed;
  boolean musicButtonHold;
};

RawInput lastCycleRawInput = {
  musicButton: false
};

RawInput currentRawInput = {
  musicButton: false
};

ProcessedInput processedInput = {
  musicButtonPressed: false,
  musicButtonHold: false
};

unsigned long musicButtonStartHold = 0;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  readMatInput();
  processMatInput();
  if(processedInput.musicButtonPressed) {
    toggleMusic();
  }
}

void readMatInput() {
  currentRawInput = {
    musicButton: digitalRead(musicButtonPin)
  };
}

void processMatInput() {
  processedInput = {
    musicButtonPressed: false,
    musicButtonHold: false
  };
  
  //Music button
  if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == LOW) {
    processedInput.musicButtonPressed = true;
  } else if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == HIGH &&
        millis() - musicButtonStartHold > 1000) {
    processedInput.musicButtonHold = true;
  } else if(lastCycleRawInput.musicButton == LOW && currentRawInput.musicButton == HIGH) {
    musicButtonStartHold = millis();
  }


  lastCycleRawInput = currentRawInput;
}

