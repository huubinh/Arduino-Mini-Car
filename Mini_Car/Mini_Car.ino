
#include <IRremote.h>
#define THRESHOLD 600
#define L1 9
#define L2 8
#define R1 5
#define R2 4
#define IRPIN A0
#define btUP 0x18
#define btDOWN 0x52
#define btLEFT 0x8
#define btRIGHT 0x5A
#define RUNNING 0x0
#define btOK 0x1C
#define btNUM1 0x45
#define btNUM2 0x46
#define btNUM3 0x47
#define btSpeedUp 0x16
#define btSlowDown 0xD
#define BLUEPIN 3
#define GREENPIN 6
#define REDPIN 10

#define WHITE 0b000
#define LIGHTBLUE  0b001
#define PINK 0b010
#define DARKBLUE 0b011
#define LIGHTGREEN 0b100
#define DARKGREEN 0b101
#define RED 0b110

typedef enum {STOPC, FORWARD, BACKWARD, LEFT, RIGHT} CarState;
typedef enum {STOPW, CW, CCW} WheelState;
typedef enum {L, R} Wheel;
byte speed = 150;
unsigned long runtime = 0;
bool running = false;
byte mode = 1;

void controlCar(CarState s);
void controlWheel(Wheel w, WheelState s );
void IRControl();
void LDRControl();
void BluetoothControl();
void RGBLed(byte color);

void setup() {
  pinMode(L2, OUTPUT);
  pinMode(R2, OUTPUT);
  controlCar(STOPC);
  Serial.begin(9600);
  IrReceiver.begin(IRPIN);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);

  for (int i = 6; i >= 0; i--) {
    RGBLed(i);
    delay(250);
  }
  RGBLed(PINK);
}

void loop() {
  IRControl();
  if (mode == 2)
    LDRControl();
  if (mode == 3)
    BluetoothControl();
}

void controlCar(CarState s) {
  switch (s) {
    case STOPC:
      controlWheel(L, STOPW);
      controlWheel(R, STOPW);
      break;
    case FORWARD:
      controlWheel(L, CW);
      controlWheel(R, CW);
      break;
    case BACKWARD:
      controlWheel(L, CCW);
      controlWheel(R, CCW);
      break;
    case LEFT:
      controlWheel(L, STOPW);
      controlWheel(R, CW);
      break;
    case RIGHT:
      controlWheel(L, CW);
      controlWheel(R, STOPW);
  }
}

void controlWheel(Wheel w, WheelState s ) {
  switch (s) {
    case STOPW:
      if ( w == L ) {
        analogWrite(L1, 0);
        digitalWrite(L2, 0);
      }
      else {
        analogWrite(R1, 0);
        digitalWrite(R2, 0);
      }
      break;
    case CW:
      if ( w == L ) {
        analogWrite(L1, speed);
        digitalWrite(L2, 0);
      }
      else {
        analogWrite(R1, speed);
        digitalWrite(R2, 0);
      }
      break;
    case CCW:
      if ( w == L ) {
        analogWrite(L1, 255 - speed);
        digitalWrite(L2, 1);
      }
      else {
        analogWrite(R1, 255 - speed);
        digitalWrite(R2, 1);
      }
      break;
  }
}

void IRControl() {
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    switch (IrReceiver.decodedIRData.command) {
      case btNUM1:
        if (mode == 1) break;
        mode = 1;
        RGBLed(PINK);
        break;
      case btNUM2:
        if (mode == 2) break;
        mode = 2;
        RGBLed(DARKGREEN);
        break;
      case btNUM3:
        if (mode == 3) break;
        mode = 3;
        RGBLed(LIGHTBLUE);
        break;
        
      case btUP:
        if (mode != 1) break;
        controlCar(FORWARD);
        runtime = millis();
        running = true;
        break;
      case btDOWN:
        if (mode != 1) break;
        controlCar(BACKWARD);
        runtime = millis();
        running = true;
        break;
      case btLEFT:
        if (mode != 1) break;
        controlCar(LEFT);
        runtime = millis();
        running = true;
        break;
      case btRIGHT:
        if (mode != 1) break;
        controlCar(RIGHT);
        runtime = millis();
        running = true;
        break;
      case RUNNING:
        runtime = millis();
        running = true;
        break;
        
      case btSpeedUp:
        if (speed >= 110)
          speed -= 10;
        break;
      case btSlowDown:
        if (speed <= 240)
          speed += 10;
    }
    IrReceiver.resume();
  }
  if ( millis() - runtime > 120 && running) {
    controlCar(STOPC);
    running = false;
  }
}

void LDRControl() {
  int up = analogRead(A1);
  int down = analogRead(A2);
  int left = analogRead(A3);
  int right = analogRead(A4);
  //Serial.println(String(up) + " " + String(down) + " " + String(left) + " " + String(right));
  if (up > THRESHOLD)
    controlCar(FORWARD);
  else if (down > THRESHOLD)
    controlCar(BACKWARD);
  else if (left > THRESHOLD)
    controlCar(LEFT);
  else if (right > THRESHOLD)
    controlCar(RIGHT);
  else
    controlCar(STOPC);
}

void BluetoothControl() {
  if (Serial.available()) {
    switch (Serial.read()) {
      case 'F':
        controlCar(FORWARD);
        break;
      case 'B':
        controlCar(BACKWARD);
        break;
      case 'L':
        controlCar(LEFT);
        break;
      case 'R':
        controlCar(RIGHT);
        break;
      case 'S':
        controlCar(STOPC);
    }
  }
}

void RGBLed(byte color) {
  digitalWrite(BLUEPIN, (color >> 2));
  digitalWrite(GREENPIN, (color >> 1) & 1);
  digitalWrite(REDPIN, color & 1);
}
