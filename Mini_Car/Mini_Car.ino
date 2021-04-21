//#include <TimerOne.h>
#include <IRremote.h>
#define THRESHOLD 700
#define L1 9
#define L2 8
#define R1 5
#define R2 4
#define IRPIN A0
#define btUP 0x18
#define btDOWN 0x52
#define btLEFT 0x8
#define btRIGHT 0x5A
#define btOK 0x1C
#define btNUM1 0x45
#define btNUM2 0x46
#define btNUM3 0x47

typedef enum {STOPC, FORWARD, BACKWARD, LEFT, RIGHT} CarState;
typedef enum {STOPW, CW, CCW} WheelState;
typedef enum {L, R} Wheel;
byte speed = 120;
unsigned long runtime = 0;
bool running = false;
byte mode = 1;
void Mode();
void controlCar(CarState s);
void controlWheel(Wheel w, WheelState s );
void IRControl();
void LDRControl();
void BluetoothControl();

void setup() {
  pinMode(L2, OUTPUT);
  pinMode(R2, OUTPUT);
  controlCar(STOPC);
  Serial.begin(9600);
  IrReceiver.begin(IRPIN);
  //  Timer1.initialize(1000);
  //  Timer1.attachInterrupt(Mode);
}

void loop() {
  //  switch (mode) {
  //    case 1:
  //      IRControl();
  //      break;
  //    case 2:
  //      LDRControl();
  //      break;
  //    case 3:
  //      BluetoothControl();
  //  }
  IRControl();
  BluetoothControl();
}

void Mode() {
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    switch (IrReceiver.decodedIRData.command) {
      case btNUM1:
        mode = 1;
        break;
      case btNUM2:
        mode = 2;
        break;
      case btNUM3:
        mode = 3;
    }
    IrReceiver.resume();
  }
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
    //Serial.println(IrReceiver.decodedIRData.command, HEX);
    switch (IrReceiver.decodedIRData.command) {
      case btUP:
        controlCar(FORWARD);
        break;
      case btDOWN:
        controlCar(BACKWARD);
        break;
      case btLEFT:
        controlCar(LEFT);
        break;
      case btRIGHT:
        controlCar(RIGHT);
    }
    runtime = millis();
    running = true;
    IrReceiver.resume();
  }
  if ( millis() - runtime > 100 && running) {
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
