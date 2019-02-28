int irAnalogPin = A0;
int restAreaButtonPin = 0;
int coffeeButtonPin = 1;
int musicButtonPin = 2;
int lightButtonPin = 3;
int nightLightButtonPin = 4;

int nightLightOutputPin = 5;
int lightstripClockPin = 6;
int lightstripOutputPin = 7;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  turnOnIotLight();
}
