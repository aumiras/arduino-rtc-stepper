/**
     * Author:Ab Kurk   version: 1.0    date: 4/02/2018
 * Description: This sketch is part of the guide to putting your Arduino to sleep tutorial. We use the:
 * Adafruit DS3231 RTC. In this example we use the RTC to wake up the Arduino to do something. After the operations has been done the Arduino goes back to sleep and gets woken up 5 minutes later to start all over again. Link To Tutorial http://www.thearduinomakerman.info/blog/2018/1/24/guide-to-arduino-sleep-mode Link To Project   http://www.thearduinomakerman.info/blog/2018/2/5/wakeup-rtc-datalogger
 */

#include <avr/sleep.h>  //this AVR library contains the methods that controls the sleep modes
#include <DS3232RTC.h>  //RTC Library https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>
#include <Arduino.h>
#include <Wire.h>
//#include <RTClib.h>
#include <StepperMotor.h>

// Pin Definitions

#define interruptPin 2 //Pin we are going to use to wake up the Arduino

int smDirectionPin = 4; //Direction pin
int smStepPin = 5; //Stepper pin



const int time_interval=1;  // Sets the wakeup intervall in minutes

/*
const int timeout = 10000;       //define timeout of 10 sec
char menuOption = 0;
long time0;
*/

void setup() {
  Serial.begin(9600);//Start Serial Comunication
  pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indecate when Arduino is A sleep
  pinMode(interruptPin,INPUT_PULLUP);//Set pin d2 to input using the buildin pullup resistor
  digitalWrite(LED_BUILTIN,HIGH);//turning LED on
  
  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);

    Serial.print("initialize the alarms"); Serial.print("\t"); Serial.print("clear the alarm flags"); Serial.print("\t"); Serial.println("clear the alarm interrupt flags");
    
    /////////// set the RTC time and date to the compile time
    //setTime(23, 05, 00, 20, 6, 2019);   //set the system time to 23h31m30s on 13Feb2009
  //RTC.set(now());
    
    time_t t; //create a temporary time variable so we can set the time and read the time from the RTC
    t=RTC.get();//Gets the current time of the RTC

    Serial.println(t);
    Serial.println(minute(t));
    Serial.println(time_interval);

    
    // SYNTAXE    
    // RTC.setAlarm(alarmType, seconds, minutes, hours, dayOrDate);
    RTC.setAlarm(ALM1_MATCH_SECONDS, 0, minute(t)+time_interval, 0, 0);  // Setting alarm 1 to go off 5 minutes from now
    //RTC.setAlarm(ALM1_MATCH_HOURS, hour(t)+time_interval, 0, 0, 0);  // Setting alarm 1 to go off 5 minutes from now
    Serial.print("ALM1_MATCH_MINUTES"); Serial.print("\t"); Serial.print(hour(t)+time_interval); Serial.println("//***//");
    
    // clear the alarm flag
    RTC.alarm(ALARM_1);
    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
    RTC.squareWave(SQWAVE_NONE);
    Serial.print("INT/SQW pin"); Serial.print("\t"); Serial.print("- interrupt operation"); Serial.print("\t"); Serial.println("- DISABLE");
    
    // enable interrupt output for Alarm 1
    RTC.alarmInterrupt(ALARM_1, true);
    Serial.print("set ALARM_1"); Serial.print("\t"); Serial.print("- enable interrupt"); Serial.print("\t"); Serial.println("- TRUE");

    pinMode(smDirectionPin, OUTPUT);
    pinMode(smStepPin, OUTPUT);
    
    Serial.println("initialization done.");
    Serial.println("--------------------------");
    Serial.println("");
}

void loop() {
 delay(5000);//wait 5 seconds before going to sleep. In real senairio keep this as small as posible
 Going_To_Sleep();
}

void Going_To_Sleep(){
    sleep_enable();//Enabling sleep mode
    attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    digitalWrite(LED_BUILTIN,LOW);//turning LED off
    time_t t;// creates temp time variable
    t=RTC.get(); //gets current time from rtc
    Serial.println("Sleep  Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//prints time stamp on serial monitor
    delay(1000); //wait a second to allow the led to be turned off before going to sleep
    sleep_cpu();//activating sleep mode
    Serial.println("just woke up!");//next line of code executed after the interrupt 
    digitalWrite(LED_BUILTIN,HIGH);//turning LED on
    
    motorRotate();
    //temp_Humi();//function that reads the temp and the humidity
    
    t=RTC.get();
    Serial.println("WakeUp Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//Prints time stamp 
    //Set New Alarm
    RTC.setAlarm(ALM1_MATCH_SECONDS , 0, minute(t)+time_interval, 0, 0);
  
  // clear the alarm flag
  RTC.alarm(ALARM_1);
  }

void wakeUp(){
  Serial.println("Interrrupt Fired");//Print message to serial monitor
   sleep_disable();//Disable sleep mode
  detachInterrupt(0); //Removes the interrupt from pin 2;
 
}

void motorRotate(){
  Serial.println("Motor running");
rotate(3200, 1); //The motor rotates 1600 steps clockwise with a speed of 0.5 (medium)
  delay(5000);
  Serial.println("Done running");
  }

  /*The rotate function turns the stepper motor. Tt accepts two arguments: 'steps' and 'speed'*/
void rotate(int steps, float speed){
  /*This section looks at the 'steps' argument and stores 'HIGH' in the 'direction' variable if */
  /*'steps' contains a positive number and 'LOW' if it contains a negative.*/
  int direction;
 
  if (steps > 0){
    direction = HIGH;
  }else{
    direction = LOW;
  }
 
  speed = 1/speed * 70; //Calculating speed
  steps = abs(steps); //Stores the absolute value of the content in 'steps' back into the 'steps' variable
 
  digitalWrite(smDirectionPin, direction); //Writes the direction (from our if statement above), to the EasyDriver DIR pin
 
  /*Steppin'*/
  for (int i = 0; i < steps; i++){
    digitalWrite(smStepPin, HIGH);
    delayMicroseconds(speed);
    digitalWrite(smStepPin, LOW);
    delayMicroseconds(speed);
  }
}
