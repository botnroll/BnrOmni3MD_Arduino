/*
 Omni-3MD 3 motor controller from botnroll.com http://www.botnroll.com/en/controllers/84-bot-n-roll-omni-3md-.html
 
 Move 1 motor using PID for speed control using Omni3MD:
 - this code assumes you are using 1,2 or 3 DC motors with encoders attached to Omni3MD board;
 - this example was developed using Arduino 1.7.10 open-source Arduino environment
 - this example is compatible with Omni3MD firmware version 1.74 or higher. Visit http://www.botnroll.com/omni3md/ to check if your version is updated.

 Purpose:
  This example demonstrates linear movement with PID control:
 
 Send movement orders: 
   Function: mov1mpid( motor, speed )
   - motor: the motor to operate (1, 2 or 3);
   - speed: the desired speed for the movement (-100 to +100). 
   Ex: omni.mov1mpid(2,35);
   The motor runs at the desired speed, speed=0 stop the motor with holding torque.

 Note:
 You can use encoder readings with linear movement
 In this example encoder values are printed on serial monitor as motors are running.
 
 IMPORTANT!
 Before you upload this example, you must calibrate and configure PID control parameters for you system. 
 
   Follow these steps:
 
   1-Use the calibration routine, setting the function parameters to 0 or 1, to define the desired positive rotation for each motor.
     Ex: omni.calibrate(1,0,0);
     
   2-Set the PID control parameters kp, ki and kd
     Function: setPid( Kp, Ki, Kd )
     - kp: Proportional gain. For kp=1 input the value 1000, for kp=0.5 input the value 500. Positive values only!
     - ki: Integral gain. For ki=1 input the value 1000, for ki=0.5 input the value 500. Positive values only!
     - kd: Differential gain. For kd=1 input the value 1000, for kl=0.5 input the value 500. Positive values only!
     Ex:omni.setPid(700,400,200);
     
   3-Adjust the acceleration ramp if necessary:
     Function: setRamp( slope , kl )
     - slope: defines the acceleration of the motors (ramp inclination) varies from 0 to 100. Lower values slower acceleration.
     - kl: factor for the the necessary power to overcome the motor gearbox inertia, obtained during calibration from a system with encoders!
       Power*kl is loaded for the integral error of the PID control, under certain conditions, by the control algorithm.
       Positive values between 0 and 1000 for kl. The value 0 disables this function.
   
 
 Important routines in this example:
      //setup routines
      void i2cConnect(byte omniAddress);
      void calibrate(byte way1,byte way2,byte way3);
      void setPid(int Kp, int Ki, int Kd);
      void setRamp(int slope, int Kl);

      
      //movement routines
      void mov1mPid(byte motor,int speed);

 
 Omni3MD board features: 
 - Holonomic movement integrated control of 3 motors with PID control;
 - Differential movement (SI units) with PID control;
 - Linear control of 3 independent motors with or without PID control;
 - Independent positional control using encoder pre-sets and readings;
 - Encoder readings;
 - Battery reading;
 - Temperature reading;
 
 
 The circuit:
 * Omni3MD board attached to Arduino analog input 4 (SDA) and 5 (SCL), GND and 5V DC.
 
 This example was created by José Cruz (www.botnroll.com)
 on 27 September 2012
 Updated on 17 January 2017
 
 This code example is in the public domain. 
 http://www.botnroll.com
*/
 
#include <Wire.h>                   // required by BnrOmni.cpp
#include <BnrOmni.h>

//constants definitions
#define OMNI3MD_ADDRESS 0x30        // default factory address
#define M1  1  //Motor1
#define M2  2  //Motor2
#define M3  3  //Motor3

BnrOmni omni;  //declaration of object variable to control the Omni3MD

//Variables to read from Omni3MD
int enc1=0;    // encoder1 reading, this is the encoder incremental count for the defined prescaler (positional control)
int enc2=0;    // encoder2 reading, this is the encoder incremental count for the defined prescaler (positional control)
int enc3=0;    // encoder3 reading, this is the encoder incremental count for the defined prescaler (positional control)

void setup()
{
    //setup routines
    delay(1500);                     // wait electronics to stabilize
    Serial.begin(57600);             // set baud rate to 57600bps for printing values on serial monitor. Press (ctrl+shift+m) after uploading
    omni.i2cConnect(OMNI3MD_ADDRESS);// set i2c connection
    omni.stop();                     // stop all motors
    omni.setI2cTimeout(0);           // safety parameter -> I2C communication must occur every [byte timeout] x 100 milliseconds for motor movement                             
    omni.setPid(1000,400,500);       // adjust parameters for Speed PID control [word Kp, word Ki, word Kd]
    omni.setRamp(35,900);            // set acceleration ramp and limiar movement parameter gain[int slope, int Kl] slope between 0 and 100 kl->gain for necessary motor power to start movement
    omni.setMinBat(12.0);            // battery discharge protection voltage (Lithium 4S)
    omni.setEncPrescaler(M1, 0);     // set the prescaler to 0; encoder count will increment every 1 pulse [byte encoder, byte value]
    omni.setEncPrescaler(M2, 0);     // set the prescaler to 0; encoder count will increment every 1 pulse [byte encoder, byte value]
    omni.setEncPrescaler(M3, 0);     // set the prescaler to 0; encoder count will increment every 1 pulse [byte encoder, byte value]
//    omni.calibrate(1,1,1);         // send the calibration command to configure the OMmni3MD board
//    delay(20000);                  // wait about 20s for calibration to end
    
    Serial.print("Firmware:");  
    Serial.println(omni.readFirmware());      // print firmware value
    Serial.print("ControlRate:");
    Serial.println(omni.readControlRate());   // print control rate value
    Serial.print("Enc1max:");
    Serial.println(omni.readEnc1Max());       // print encoder1 maximum calibration value
    Serial.print("Enc2max:");
    Serial.println(omni.readEnc2Max());       // print encoder2 maximum calibration value 
    Serial.print("Enc3max:");
    Serial.println(omni.readEnc3Max());       // print encoder3 maximum calibration value
    Serial.print("Battery:");
    Serial.println(omni.readBattery());       // print battery value
    Serial.print("Temperature:");
    Serial.println(omni.readTemperature());   // print temperature value 
}

void loop()
{
    //Variables for motion control
    int speed1=20;
    int speed2=30;
    int speed3=40;
    int preset1=0;
    int preset2=0;
    int preset3=0;

    //Encoder preset
    omni.setEncValue(M1,preset1);         // set the encoder value [byte encoder, word encValue]
    omni.setEncValue(M2,preset2);         // set the encoder value [byte encoder, word encValue]
    omni.setEncValue(M3,preset3);         // set the encoder value [byte encoder, word encValue]
    omni.readEncoders(&enc1,&enc2,&enc3); // read all encoders at once (in a single I2C request) 
    print_enc_values();                   // print values on serial monitor
    delay(1000);
    
    //Move motors    
    omni.mov1mPid(M1,speed1);    // move motor1 at speed1
    omni.mov1mPid(M2,speed2);    // move motor2 at speed2
    omni.mov1mPid(M3,speed3);    // move motor3 at speed3
    
    wait(20);     // wait 4 seconds while printing in the serial monitor
    omni.stop();  // stop all motors without holding torque
    wait(10);     // wait 2 seconds while printing in the serial monitor  

    omni.mov1mPid(M1,-speed3);   // move motor1 at speed1
    omni.mov1mPid(M2,-speed3);   // move motor2 at speed2
    omni.mov1mPid(M3,-speed3);   // move motor3 at speed3
    
    while(enc1>0 || enc2>0 || enc3>0)
    {
        omni.readEncoders(&enc1,&enc2,&enc3); // read all encoders at once (in a single I2C request) 
        print_enc_values();                   // print values on serial monitor
        delay(200);
    }
    omni.mov1mPid(M1,0); // stop motor with holding torque
    omni.mov1mPid(M2,0); // stop motor with holding torque
    omni.mov1mPid(M3,0); // stop motor with holding torque
    wait(10);            // wait 2 seconds while printing in the serial monitor 
}

//Print values on serial monitor
void print_enc_values()
{
     Serial.print("enc1:");
     Serial.print(enc1);           // print encoder 1 positional value on serial monitor
     Serial.print(" enc2:");
     Serial.print(enc2);           // print encoder 2 positional value on serial monitor
     Serial.print(" enc3:");
     Serial.println(enc3);         // print encoder 3 positional value on serial monitor
}

void wait(int time)
{
    Serial.println();
    for(int i=0;i<time;i++)    // 10->2s
    {
        omni.readEncoders(&enc1,&enc2,&enc3);
        print_enc_values();    // print values on serial monitor
        delay(200);
    }
}

