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
  boolean coffeeButton;
};

struct ProcessedInput {
  boolean musicButtonPressed;
  boolean musicButtonHold;
  boolean coffeeButtonPressed;
};

RawInput lastCycleRawInput = {
  musicButton: false,
  coffeeButton: false
};

RawInput currentRawInput = {
  musicButton: false,
  coffeeButton: false
};

ProcessedInput processedInput = {
  musicButtonPressed: false,
  musicButtonHold: false,
  coffeeButtonPressed: false
};

unsigned long musicButtonStartHold = 0;

void setup() {
  pinMode(coffeeButtonPin, INPUT);
  pinMode(musicButtonPin, INPUT);
}

void loop() {
  readMatInput();
  processMatInput();
  if(processedInput.musicButtonPressed) {
    toggleMusic();
  }
  if(processedInput.musicButtonHold) {
    changeVolume();
  }
  if(processedInput.coffeeButtonPressed) {
    toggleCoffee();
  }
}

void readMatInput() {
  currentRawInput = {
    musicButton: digitalRead(musicButtonPin),
    coffeeButton: digitalRead(coffeeButtonPin)
  };
}

void processMatInput() {
  processedInput = {
    musicButtonPressed: false,
    musicButtonHold: false,
    coffeeButtonPressed: false
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


  //Coffee button
  if(lastCycleRawInput.musicButton == HIGH && currentRawInput.musicButton == LOW) {
    processedInput.coffeeButtonPressed = true;
  }

  lastCycleRawInput = currentRawInput;
}
