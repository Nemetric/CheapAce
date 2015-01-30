

#include "XBOXRECV.h"
#include "SimpleTimer.h"
#include "PID_v1.h"
#include "Encoder.h"
#include <Servo.h> 

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

const int _ESC_Left_PWM = 5;
const int _ESC_Right_PWM = 6;
const int _ESC_Lift_PWM = 7;

/*
const int _ENC_Left_Dig = 7;
const int _ENC_Left_Int = 7;

const int _ENC_Right_Dig = 7;
const int _ENC_Right_Int = 7;

const int _ENC_Lift_Dig = 7;
const int _ENC_Lift_Int = 7;
*/


// Timer Polling Intervals
const int _readControllerInterval = 10;
const int _writeControllerInterval = 15;
const int _writeRobotInterval = 20;
const int _readRobotInterval = 25;

//HobbyKing ESC Max & Min
//const float _ESC_HK_MAX = 180; //157.0;
//const float _ESC_HK_MIN = 0; //29.0;
const float _ESC_HK_ADD = 29;
const float _ESC_HK_MAX = 157.0+_ESC_HK_ADD;
const float _ESC_HK_MIN = 29.0-_ESC_HK_ADD;

const float _ESC_Throttle_Inc = 40;


float _leftSpeed;
float _rightSpeed;
float _liftSpeed;

float _leftPos;
float _rightPos;
float _liftPos;



USB Usb;
XBOXRECV Xbox(&Usb);

Servo ESC_Left;
Servo ESC_Right;
Servo ESC_Lift;

//Encoder ENC_Left(_ENC_Left_Int, _ENC_Left_Dig);
//Encoder ENC_Right(_ENC_Right_Int, _ENC_Right_Dig);
//Encoder ENC_Lift(_ENC_Lift_Int, _ENC_Lift_Dig);

SimpleTimer timer;



void setup() {

  // assign public state variables
  _leftSpeed = 0.0;
  _rightSpeed = 0.0;
  _liftSpeed = 0.0;

  _leftPos = 0.0;
  _rightPos = 0.0; 
  _liftPos = 0.0; 


  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  
  ESC_Left.attach(_ESC_Left_PWM);
  ESC_Right.attach(_ESC_Right_PWM);
  ESC_Lift.attach(_ESC_Lift_PWM);

  timer.setInterval(_readControllerInterval, readController);
  timer.setInterval(_writeControllerInterval, writeController);
  timer.setInterval(_readRobotInterval, readRobot);
  timer.setInterval(_writeRobotInterval, writeRobot);

  Serial.print(F("\r\nInitialization Complete"));
}


void loop() {

  timer.run();

}



void readController(){
  Usb.Task();

  if (Xbox.XboxReceiverConnected) 
  {
    for (uint8_t i = 0; i < 4; i++) 
    {
      if (Xbox.Xbox360Connected[i]) 
      {
        //Left Hat Y Axis
        if (Xbox.getAnalogHat(LeftHatY, i) > 7500 || Xbox.getAnalogHat(LeftHatY, i) < -7500) {
          _leftSpeed = 100.0 / 32767 * Xbox.getAnalogHat(LeftHatY, i); 
          
        }
        else
        {
          _leftSpeed = 0.0;
          
        }
        //Right Hat Y Axis
        if (Xbox.getAnalogHat(RightHatY, i) > 7500 || Xbox.getAnalogHat(RightHatY, i) < -7500) {
          _rightSpeed = 100.0 / 32767 * Xbox.getAnalogHat(RightHatY, i); 
        }
        else
        {
          _rightSpeed = 0.0;
        }
        //L2 Trigger
        if (Xbox.getButtonPress(L2, i)) 
        {
          _liftSpeed = 100.0 / 255 * Xbox.getButtonPress(L2, i) * -1; 
        }
        //R2 Trigger
        else if (Xbox.getButtonPress(R2, i)) 
        {
          _liftSpeed = 100.0 / 255 * Xbox.getButtonPress(R2, i); 
        }
        else
        {
          _liftSpeed = 0.0;
        }
      }
    }
  }
}

void writeController(){

}

void readRobot(){

}

void writeRobot(){
  MoveSpeed(_leftSpeed, _rightSpeed);
  LiftSpeed(_liftSpeed);
}




// ===========================================
// ROBOT METHODS
// ===========================================
float _curLeftSpeed = 93;
float _curRightSpeed = 93;
 void MoveSpeed(float leftSpeed, float rightSpeed)
 {
  //Parameters will be between -100 & 100

  //Adjust Parameters to meet ESC Criteria
  leftSpeed = map(leftSpeed, -100, 100, _ESC_HK_MIN, _ESC_HK_MAX);
  rightSpeed = map(rightSpeed, -100, 100, _ESC_HK_MIN, _ESC_HK_MAX);

/*
  if(leftSpeed > _curLeftSpeed)
  {
      if(_curLeftSpeed + _ESC_Throttle_Inc >= leftSpeed)
      {
        _curLeftSpeed = leftSpeed;
      }
      else
      {
        _curLeftSpeed += _ESC_Throttle_Inc;
      }
  }
  else if(leftSpeed < _curLeftSpeed)
  {
      if(_curLeftSpeed - _ESC_Throttle_Inc <= leftSpeed)
      {
        _curLeftSpeed = leftSpeed;
      }
      else
      {
        _curLeftSpeed -= _ESC_Throttle_Inc;
      }
  }

  if(rightSpeed > _curRightSpeed)
  {
      if(_curRightSpeed + _ESC_Throttle_Inc >= rightSpeed)
      {
        _curRightSpeed = rightSpeed;
      }
      else
      {
        _curRightSpeed += _ESC_Throttle_Inc;
      }
  }
  else if(rightSpeed < _curRightSpeed)
  {
      if(_curRightSpeed - _ESC_Throttle_Inc <= rightSpeed)
      {
        _curRightSpeed = rightSpeed;
      }
      else
      {
        _curRightSpeed -= _ESC_Throttle_Inc;
      }
  }
  */

  //Set Left Speed
  ESC_Left.write(leftSpeed);

  //Set Right Speed
  ESC_Right.write(rightSpeed);

    Serial.print(F("\r\nRT: "));
    Serial.print(rightSpeed);
    Serial.print("\t LT: ");
    Serial.print(leftSpeed);
 }

 void LiftSpeed(float liftSpeed)
 {
  //Parameters will be between -100 & 100

  //Adjust Parameters to meet ESC Criteria
  liftSpeed = map(liftSpeed, -100, 100, _ESC_HK_MIN, _ESC_HK_MAX);

  //Set Left Speed
  ESC_Lift.write(liftSpeed);
 }


// ===========================================
// DELETE, only here for reference
// ===========================================
void DELETEEVENTUALLY() {
  Usb.Task();
  if (Xbox.XboxReceiverConnected) {
    for (uint8_t i = 0; i < 4; i++) {
      if (Xbox.Xbox360Connected[i]) {
        if (Xbox.getButtonPress(L2, i) || Xbox.getButtonPress(R2, i)) {
          Serial.print("L2: ");
          Serial.print(Xbox.getButtonPress(L2, i));
          Serial.print("\tR2: ");
          Serial.println(Xbox.getButtonPress(R2, i));
          Xbox.setRumbleOn(Xbox.getButtonPress(L2, i), Xbox.getButtonPress(R2, i), i);
        }

        if (Xbox.getAnalogHat(LeftHatX, i) > 7500 || Xbox.getAnalogHat(LeftHatX, i) < -7500 || Xbox.getAnalogHat(LeftHatY, i) > 7500 || Xbox.getAnalogHat(LeftHatY, i) < -7500 || Xbox.getAnalogHat(RightHatX, i) > 7500 || Xbox.getAnalogHat(RightHatX, i) < -7500 || Xbox.getAnalogHat(RightHatY, i) > 7500 || Xbox.getAnalogHat(RightHatY, i) < -7500) {
          if (Xbox.getAnalogHat(LeftHatX, i) > 7500 || Xbox.getAnalogHat(LeftHatX, i) < -7500) {
            Serial.print(F("LeftHatX: "));
            Serial.print(Xbox.getAnalogHat(LeftHatX, i));
            Serial.print("\t");
          }
          if (Xbox.getAnalogHat(LeftHatY, i) > 750 || Xbox.getAnalogHat(LeftHatY, i) < -750) {
            
            
            int val = Xbox.getAnalogHat(LeftHatY, i);
            if(val > 0)
            {
              //Map 100 - 157
              val = map(val, 750, 32767, 100, 157);
            }
            else
            {
              //Map 80 - 29
              val = map(val, -750, -32767, 80, 29);
              
            }
           // escL.write(val);
            Serial.print(F("WriteToESCLeft: "));
            Serial.print(val);
            Serial.print("\t");
          }
          
          if (Xbox.getAnalogHat(RightHatX, i) > 7500 || Xbox.getAnalogHat(RightHatX, i) < -7500) {
            Serial.print(F("RightHatX: "));
            Serial.print(Xbox.getAnalogHat(RightHatX, i));
            Serial.print("\t");
          }
          if (Xbox.getAnalogHat(RightHatY, i) > 750 || Xbox.getAnalogHat(RightHatY, i) < -750) {
            int val = Xbox.getAnalogHat(RightHatY, i);
            if(val > 0)
            {
              //Map 100 - 157
              val = map(val, 750, 32767, 100, 157);
            }
            else
            {
              //Map 80 - 29
              val = map(val, -750, -32767, 80, 29);
              
            }
       //     escR.write(val);
            Serial.print(F("WriteToESCRight: "));
            Serial.print(val);
            Serial.print("\t");
            
            
            Serial.print(F("RightHatY: "));
            Serial.print(Xbox.getAnalogHat(RightHatY, i));
          }
          Serial.println();
        }
        else
          {
      //      escL.write(90);
      //      escR.write(90);
          }

        if (Xbox.getButtonClick(UP, i)) {
          Xbox.setLedOn(LED1, i);
          Serial.println(F("Up"));
        }
        if (Xbox.getButtonClick(DOWN, i)) {
          Xbox.setLedOn(LED4, i);
          Serial.println(F("Down"));
        }
        if (Xbox.getButtonClick(LEFT, i)) {
          Xbox.setLedOn(LED3, i);
          Serial.println(F("Left"));
        }
        if (Xbox.getButtonClick(RIGHT, i)) {
          Xbox.setLedOn(LED2, i);
          Serial.println(F("Right"));
        }

        if (Xbox.getButtonClick(START, i)) {
          Xbox.setLedMode(ALTERNATING, i);
          Serial.println(F("Start"));
        }
        if (Xbox.getButtonClick(BACK, i)) {
          Xbox.setLedBlink(ALL, i);
          Serial.println(F("Back"));
        }
        if (Xbox.getButtonClick(L3, i))
          Serial.println(F("L3"));
        if (Xbox.getButtonClick(R3, i))
          Serial.println(F("R3"));

        if (Xbox.getButtonClick(L1, i))
          Serial.println(F("L1"));
        if (Xbox.getButtonClick(R1, i))
          Serial.println(F("R1"));
        if (Xbox.getButtonClick(XBOX, i)) {
          Xbox.setLedMode(ROTATING, i);
          Serial.print(F("Xbox (Battery: "));
          Serial.print(Xbox.getBatteryLevel(i)); // The battery level in the range 0-3
          Serial.println(F(")"));
        }
        if (Xbox.getButtonClick(SYNC, i)) {
          Serial.println(F("Sync"));
          Xbox.disconnect(i);
        }

        if (Xbox.getButtonClick(A, i))
          Serial.println(F("A"));
        if (Xbox.getButtonClick(B, i))
          Serial.println(F("B"));
        if (Xbox.getButtonClick(X, i))
          Serial.println(F("X"));
        if (Xbox.getButtonClick(Y, i))
          Serial.println(F("Y"));
      }
    }
  }
}
