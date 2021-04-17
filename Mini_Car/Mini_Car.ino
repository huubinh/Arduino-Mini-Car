#include <IRremote.h>
#define L1 11
#define L2 10
#define R1 9
#define R2 8
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
unsigned long runtime;
bool running = 0;
void controlCar(CarState s);
void controlWheel(Wheel w, WheelState s );
void IRControl();

void setup() {
  pinMode(L2, OUTPUT);
  pinMode(R2, OUTPUT);
  controlCar(STOPC);
  //  irrecv.enableIRIn();
  Serial.begin(9600);
  Serial.print(CW);
  IrReceiver.begin(IRPIN);
}

void loop() {
  IRControl();
  
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
        break;
    }
    runtime = millis();
    running = true;
    IrReceiver.resume();
  }
  if ( millis() - runtime > 300 && running) {
    controlCar(STOPC);
    running = false;
  }
}