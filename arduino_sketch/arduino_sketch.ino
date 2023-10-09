#include "TurboStepper.hpp"

using Timer1 = ATMEGA328P::Timer1<C2MHz>;
using Timer2 = ATMEGA328P::Timer2<C15_625Hz>;

using Arm1Stepper = Stepper<10, 9, 800, 4, 2>;
using Arm2Stepper = Stepper<8, 5, 800, 4, 2>;
using ZStepper = Stepper<6, 7, 1600, 1, 3>;

using Arm1Profile = TrapezoidalProfile<Arm1Stepper, Timer1::CounterA>;
using Arm2Profile = TrapezoidalProfile<Arm2Stepper, Timer1::CounterB>;
using ZProfile = TrapezoidalProfile<ZStepper, Timer2::CounterA>;

ISR(TIMER1_COMPA_vect) { Arm1Profile::DoStep(); }
ISR(TIMER1_COMPB_vect) { Arm2Profile::DoStep(); }
ISR(TIMER2_COMPA_vect) { ZProfile::DoStep(); }

void setup() {
    Arm1Stepper::Setup();
    Arm2Stepper::Setup();
    ZStepper::Setup();

    Timer1::Setup();
    Timer2::Setup();

    Arm1Profile::Setup();
    Arm2Profile::Setup();
    ZProfile::Setup();

    Serial.begin(19200);
    pinModeFast(2, INPUT_PULLUP);
    pinMode(3, OUTPUT);
    Serial.println("Hello");
}

void loop() { handleSerialInput(); }

void pickOrPlace(bool pick) {
    ZProfile::SetSpeed(-1.0);
    while(digitalReadFast(2) != LOW) delayMicroseconds(100);
    ZProfile::SetSpeed(0.0);
    digitalWrite(3, pick ? HIGH : LOW);
    ZProfile::WaitStop();
    ZProfile::MoveForward(-ZStepper::stepPos);
    ZProfile::WaitStop();
}

//     /\
//    A  B
//   /    \
//   ---C---
// This function returns the angle between A and B in radians
float lawOfCosines(float a, float b, float c) {
    return acosf((a * a + b * b - c * c) / (2.0f * a * b));
}
//               /
//              / \  
//             /   A2
//            /     )
//      |-A1-o----------O
//      |   /
//      |  /
//      | /
//      |/
//     / \
//    /   \
//   /     \
//   -------
// distances in mm, angles in degrees
void InverseKinematic(float x, float y, float * A0, float * A1, float * A2) {
    static const float rad2deg = 57.2958f;
    static const float arm1_len = 91.61f;
    static const float arm2_len = 105.92f;

    float dist = sqrt(x * x + y * y);

    if(dist > (arm1_len + arm2_len)) {
        dist = (arm1_len + arm2_len) - 0.001f;
        Serial.println("IK overflow->limit");
    }
    *A0 = atan2(y, x) * rad2deg;
    *A1 = lawOfCosines(dist, arm1_len, arm2_len) * rad2deg;
    *A2 = 180.0f - lawOfCosines(arm1_len, arm2_len, dist) * rad2deg;
}

// InverseKinematic(x, y, 91.61, 105.92, elbow, &A1, &A2);

void moveTo(float x, float y) {
    static bool elbow_left = true;
    float A0, A1, A2;
    InverseKinematic(x, y, &A0, &A1, &A2);

    Serial.println(A1);
    Serial.println(A2);

    // Arm1Profile::MoveTo(800 * (A0+A1)/360.0);
    // Arm2Profile::MoveTo(800 * (A2)/360.0);
    // Arm1Profile::WaitStop();
    // Arm2Profile::WaitStop();
}

void handleSerialInput() {
    if(!Serial.available()) return;
    char c = '\0';
    Serial.readBytes(&c, 1);
    if(c == 'G') {
        pickOrPlace(true);
    } else if(c == 'R') {
        pickOrPlace(false);
    } else if(c == 'M') {
        float x = Serial.parseFloat();
        float y = Serial.parseFloat();
        moveTo(x, y);
    }
    Serial.readStringUntil('\n');
}
