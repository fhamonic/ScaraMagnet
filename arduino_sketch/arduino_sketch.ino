#include "TurboStepper.hpp"

#define ENDSTOP_PIN 2
#define MAGNET_PIN 3

using Timer1 = ATMEGA328P::Timer1<C2MHz>;
using Timer2 = ATMEGA328P::Timer2<C15_625Hz>;

using Arm1Stepper = Stepper<10, 9, 800, 4, 2>;
using Arm2Stepper = Stepper<8, 5, 800, 4, 2>;
using ZStepper = Stepper<6, 7, 800, 1, 6>;

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

    Serial.begin(115200);
    pinModeFast(ENDSTOP_PIN, INPUT_PULLUP);
    pinMode(MAGNET_PIN, OUTPUT);
    Serial.println("Hello");

    
    pickOrPlace(true);
}

void loop() { 
  //handleSerialInput();  
}

void pickOrPlace(bool pick) {
    //ZProfile::SetSpeed(0.1);
    ZProfile::MoveBackward(-10000);
    while(digitalReadFast(ENDSTOP_PIN) != LOW) delayMicroseconds(100);
    ZProfile::SetSpeed(0.0);
    digitalWrite(MAGNET_PIN, pick ? HIGH : LOW);
    ZProfile::WaitStop();
    ZProfile::MoveForward(-ZStepper::stepPos);
    ZProfile::WaitStop();
}

//     /\
//    A  B
//   /    \
//   ---C---
// This function returns the angle between sides A and B in radians
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
//   /_____\
// distances in mm, angles in degrees
bool InverseKinematic(float x, float y, float * A0, float * A1, float * A2) {
    static const float rad2deg = 57.2958f;
    static const float arm1_len = 91.61f;
    static const float arm2_len = 105.92f;

    float dist = sqrt(x * x + y * y);
    if(dist > (arm1_len + arm2_len)) {
        Serial.println("Impossible to reach: too far (will try max distance)");
        dist = (arm1_len + arm2_len) - 0.001f;
    }
    if(dist < 70) {
        Serial.println("Impossible to reach: too close (cancel movement)");
        return false;
    }
    *A0 = atan2(x, y) * rad2deg;
    *A1 = lawOfCosines(dist, arm1_len, arm2_len) * rad2deg;
    *A2 = 180.0f - lawOfCosines(arm1_len, arm2_len, dist) * rad2deg;
    return true;
}

void moveToAngles(float angle_arm1, float angle_arm2) {
    static bool elbow_left = true;
    if(elbow_left && angle_arm1 < -95) elbow_left = false;
    if(!elbow_left && angle_arm1 > 95) elbow_left = true;
    if(!elbow_left) {
        angle_arm1 = -angle_arm1;
        angle_arm2 = -angle_arm2;
    }
    
    float rot1 = angle_arm1 / 360.0;
    float rot2 = angle_arm2 / 360.0;
    
    Arm1Profile::MoveTo(800 * rot1 * (72.0/20.0));
    Arm2Profile::MoveTo(800 * (rot2 * (62.0/20.0) * (62.0/35.0)  + rot1  * (62.0/20.0)));

    Arm1Profile::WaitStop();
    Arm2Profile::WaitStop();
}

void moveToPos(float x, float y) {
    float A0, A1, A2;
    if(!InverseKinematic(x, y, &A0, &A1, &A2)) return;
    moveToAngles((A0 - A1), A2);
}

void handleSerialInput() {
    if(!Serial.available()) return;
    char c = Serial.read();    
    Serial.println(c);
    if(c == 'G') {
        pickOrPlace(true);
    } else if(c == 'R') {
        pickOrPlace(false);
    } else if(c == 'M') {
        float x = Serial.parseFloat();
        float y = Serial.parseFloat();
        moveToPos(x, y);
    } else if(c == 'H') {
        moveToAngles(0,0);
    } else if(c == 'P') {
        moveToAngles(95.0f, -5.0f);
    }
}
