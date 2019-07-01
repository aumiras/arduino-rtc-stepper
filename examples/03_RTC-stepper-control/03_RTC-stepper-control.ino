/*
 The code is based on famous Ab Kurk tutorial on putting Arduino Uno to sleep with Adafruit DS3231 RTC, excluded Adafruit 5V ready Micro SD break out board and DHT11 or DHT22 humidety/Temperature sensor
 Link To Tutorial http://www.thearduinomakerman.info/blog/2018/1/24/guide-to-arduino-sleep-mode
 Link To Project   http://www.thearduinomakerman.info/blog/2018/2/5/wakeup-rtc-datalogger
 */

#include <avr/sleep.h>
#include <DS3232RTC.h>  //RTC Library https://github.com/JChristensen/DS3232RTC

const byte interruptPin = 2;
int time_interval = 1;
int smDirectionPin = 12; // stepper pins setup
int smStepPin = 13;

void setup() {
  Serial.begin(9600);
  Serial.println("initialize pins.");
  
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(interruptPin,INPUT_PULLUP);
  digitalWrite(LED_BUILTIN,HIGH);     //turning LED on

  pinMode(smDirectionPin, OUTPUT); // stepper pinMode declaration
  pinMode(smStepPin, OUTPUT);

  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);
  
    time_t t;       //create a temporary time variable so we can set the time and read the time from the RTC
    t=RTC.get();    //Gets the current time of the RTC
    int alarmDuration = minute(t)+time_interval;    // calculate Alarm minutes
    RTC.setAlarm(ALM1_MATCH_MINUTES , 0, alarmDuration, 0, 0);    // Setting alarm1 to go off time_interval from now /check code 11 line
    Serial.println(alarmDuration);    //prints Alarm minutes on serial monitor
    
    RTC.alarm(ALARM_1);   // clear the alarm flag
    RTC.squareWave(SQWAVE_NONE);    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
    Serial.println("enable interrupt output for Alarm 1.");   // enable interrupt output for Alarm 1
    RTC.alarmInterrupt(ALARM_1, true);

    Serial.println("initialization done.");
}

void loop() {
 delay(5000);   //  wait 5 seconds before going to sleep. In real scenairio keep this as small as posible
 Going_To_Sleep();
}

void Going_To_Sleep(){
    sleep_enable();   //  Enabling sleep mode
    attachInterrupt(digitalPinToInterrupt(interruptPin), wakeUp, LOW);    //  attachInterrupt(0, wakeUp, LOW);//attaching a interrupt to pin d2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    digitalWrite(LED_BUILTIN,LOW);//turning LED off
    time_t t;// creates temp time variable
    t=RTC.get(); //gets current time from rtc
    Serial.println("Sleep  Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//prints time stamp on serial monitor
    delay(1000); //wait a second to allow the led to be turned off before going to sleep
    sleep_cpu();//activating sleep mode
    
    Serial.println("just woke up!");//next line of code executed after the interrupt
    digitalWrite(LED_BUILTIN,HIGH);//turning LED on

    t=RTC.get();
    Serial.println("WakeUp Time: "+String(hour(t))+":"+String(minute(t))+":"+String(second(t)));//Prints time stamp

    Serial.println("Motor begin");
  rotate(22500, 0.5);
  delay(5000);
    
    //Set New Alarm
    t=RTC.get();
    int alarmDuration = minute(t)+time_interval;
    RTC.setAlarm(ALM1_MATCH_MINUTES , 0, minute(t)+time_interval, 0, 0);
    Serial.print("Next wakeUp Call: ");
    Serial.println(alarmDuration);

  // clear the alarm flag
  RTC.alarm(ALARM_1);
  
  }

void wakeUp(){
  Serial.println("");
  Serial.println("wakeUp function start: ");//Print message to serial monitor
   sleep_disable();//Disable sleep mode
  detachInterrupt(0); //Removes the interrupt from pin 2;
}

void rotate(int steps, float speed){
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
    Serial.print(i);
    Serial.print(" ");
  }
  Serial.println("Motor run end");
}
