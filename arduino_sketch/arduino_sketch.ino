#include "TurboStepper.hpp"

using Timer1 = ATMEGA328P::Timer1<C2MHz>;
using Timer2 = ATMEGA328P::Timer2<C15_625Hz>;

using Arm1Stepper = Stepper<10,9, 800, 4, 2>;
using Arm2Stepper = Stepper<8,5, 800, 4, 2>;
using ZStepper = Stepper<6,7, 1600, 1, 3>;

using Arm1Profile = TrapezoidalProfile<Arm1Stepper, Timer1::CounterA>;
using Arm2Profile = TrapezoidalProfile<Arm2Stepper, Timer1::CounterB>;
using ZProfile = TrapezoidalProfile<ZStepper, Timer2::CounterA>;

ISR(TIMER1_COMPA_vect) { Arm1Profile::DoStep(); }
ISR(TIMER1_COMPB_vect) { Arm2Profile::DoStep(); }
ISR(TIMER2_COMPA_vect) { ZProfile::DoStep(); }


#define INPUT_BUFFER_SIZE 64
static char inputBuffer[INPUT_BUFFER_SIZE];


// sudo chmod a+rw /dev/ttyACM0


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
    pinMode(LED_BUILTIN, OUTPUT);
}



void loop() { 
  
    ZProfile::SetSpeed(-1.0);
    while(digitalReadFast(2) != LOW) delayMicroseconds(100);
    ZProfile::SetSpeed(0.0);
    while(ZProfile::IsMoving());
    ZProfile::MoveForward(-ZStepper::stepPos);
    
    
  while(true) {
    
    /*
    Arm1Profile::MoveForward(1600);
    Arm2Profile::MoveBackward(1600);
    
    ZProfile::MoveForward(4600);
    
    while(Arm1Profile::IsMoving() || Arm2Profile::IsMoving() || ZProfile::IsMoving());
    Arm1Profile::MoveBackward(1600);
    Arm2Profile::MoveForward(1600);
    ZProfile::MoveBackward(4600);
    while(Arm1Profile::IsMoving() || Arm2Profile::IsMoving() || ZProfile::IsMoving());
*/
    
  }
  //handleSerialInput(); 

}

void handleSerialInput() {
    if(!Serial.available()) return;
    size_t byte_count =
        Serial.readBytesUntil('\n', inputBuffer, INPUT_BUFFER_SIZE);
    inputBuffer[byte_count] = '\0';
    interpretCommand(inputBuffer, byte_count);
}

void interpretCommand(char * inputBuffer, byte commandLength) {
    if(inputBuffer[0] == 'P') {
    } else if(inputBuffer[0] == '0') {
        Serial.println("OK");
    } else if(inputBuffer[0] == '1') {
        Serial.println("OK");
    } else {
        Serial.print("Unknown command: '");
        Serial.print(inputBuffer);
        Serial.println("'");
    }
    Serial.flush();
}
