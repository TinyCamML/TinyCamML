/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/efarquh/Documents/GitHub/Boron-and-OpenMV/Firmware/Boron/src/TinyCamPublish.ino"
//Include Particle Device OS APIs
#include "Particle.h"

void setup();
void loop();
int secondsUntilNextEvent();
#line 4 "c:/Users/efarquh/Documents/GitHub/Boron-and-OpenMV/Firmware/Boron/src/TinyCamPublish.ino"
long real_time;
int millis_now;

//------------------State variables
// not yet used but placeholders in case of additional states
enum State {
  DATALOG_STATE,
  PUBLISH_STATE,
  SLEEP_STATE
};
State state = DATALOG_STATE;

// Define whether (1) or not (0) to publish
#define PUBLISHING 1

unsigned long stateTime = 0;
char data[120];

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(SEMI_AUTOMATIC);

// Global objects
FuelGauge batteryMonitor;
const char * eventName = "FloodorNo";

SystemSleepConfiguration config;

// Various timing constants
const unsigned long MAX_TIME_TO_PUBLISH_MS = 20000; // Only stay awake for this time trying to connect to the cloud and publish
// const unsigned long TIME_AFTER_PUBLISH_MS = 4000; // After publish, wait 4 seconds for data to go out
const unsigned long SECONDS_BETWEEN_MEASUREMENTS = 360; // What should sampling period be?
const unsigned long EARLYBIRD_SECONDS = 0; // how long before desired time should I wake up? 
const unsigned long TIMEOUT_TINYCAM_MS = 5000;

void setup() {

  if (PUBLISHING == 1) {
    Particle.connect();
  } else {
    Cellular.off(); // turn off cellular for prelim testing (uncomment)
  }

  Serial.begin(9600);
  Serial1.begin(9600); // Initialize serial communication
  pinMode(A0, OUTPUT); 
  digitalWrite(A0, HIGH); 

  //Serial1.begin(9600); //*** do we even need this? 
  Serial1.setTimeout(TIMEOUT_TINYCAM_MS);
  
}

void loop() {
  // Enter state machine
  switch (state) {

  case DATALOG_STATE: {
    digitalWrite(A0, LOW);
    delay(1000);
    digitalWrite(A0, HIGH);
    digitalWrite(A0, LOW);
    delay(2000);

    String statement = Serial1.readString();
    Serial.println(statement); 
    real_time = Time.now();
    digitalWrite(A0, HIGH);
    float voltage = analogRead(A1) * .00133;
    // where .00133 = 3.3V / 4096 counts * ((R1 + R2) / R2) * ((R1 + R2) / R2) where R1 and R2 are in ohms 
    // e.g., analogRead(A1) * ((3.3/4096)*((2000000+1300000)/2000000)) 

    Serial1.end();
    delay(1000);
    Serial1.begin(9600);
    Serial1.print(real_time); //Send its datetime to the openmv
    Serial.println(real_time);
    delay(1000);
    snprintf(data, sizeof(data), "%li,%s,%.02f", //,%.5f,%.5f,%.5f,%.5f,%.5f,%.02f,%.02f",
      real_time, statement.c_str(), voltage // if it takes a while to connect, this time could be offset from sensor recording
       
    );

    // Print out data buffer
    Serial.println(data);

    if (PUBLISHING == 1) {
      state = PUBLISH_STATE;
    } else {
      state = SLEEP_STATE;
    }

  }

  break;

  case PUBLISH_STATE: {

    // Prep for cellular transmission
    bool isMaxTime = false;
    stateTime = millis();

    
    // Clean out any residual junk in buffer and restart serial port
    Serial1.end();
    delay(1000);
    Serial1.begin(9600);
    delay(500);
    Serial1.setTimeout(TIMEOUT_TINYCAM_MS);

    while (!isMaxTime) {
      //connect particle to the cloud
      if (Particle.connected() == false) {
        Particle.connect();
        Serial.print("Trying to connect");
      }

      // If connected, publish data buffer
      if (Particle.connected()) {

        Serial.println("publishing data");

        // bool (or Future) below requires acknowledgment to proceed
        bool success = Particle.publish(eventName, data, 60, PRIVATE, WITH_ACK);
        Serial.printlnf("publish result %d", success);

        isMaxTime = true;
        state = SLEEP_STATE;
      }
      // If not connected after certain amount of time, go to sleep to save battery
      else {
        // Took too long to publish, just go to sleep
        if (millis() - stateTime >= MAX_TIME_TO_PUBLISH_MS) {
          isMaxTime = true;
          state = SLEEP_STATE;
          Serial.println("max time for publishing reached without success; go to sleep");
        }

        Serial.println("Not max time, try again to connect and publish");
        delay(500);
      }
    }
  }

  break;

  //Ready to sleep
  case SLEEP_STATE: {
    Serial.println("going to sleep");
    delay(500);

    // Sleep time determination and configuration
    int wakeInSeconds = secondsUntilNextEvent(); // Calculate how long to sleep 

    config.mode(SystemSleepMode::ULTRA_LOW_POWER)
      .gpio(D2, FALLING)
      .duration(wakeInSeconds * 1000L) // Set seconds until wake
      .network(NETWORK_INTERFACE_CELLULAR, SystemSleepNetworkFlag::INACTIVE_STANDBY); // keeps the cellular modem powered, but does not wake the MCU for received data

    // Ready to sleep
    SystemSleepResult result = System.sleep(config); // Device sleeps here

    // It'll only make it here if the sleep call doesn't work for some reason (UPDATE: only true for hibernate. ULP will wake here.)
    Serial.print("Feeling restless");
    stateTime = millis();
    state = DATALOG_STATE;
  }
  break;
  }
}

int secondsUntilNextEvent() {

  int current_seconds = Time.now();
  int seconds_to_sleep = SECONDS_BETWEEN_MEASUREMENTS - (current_seconds % SECONDS_BETWEEN_MEASUREMENTS) - EARLYBIRD_SECONDS;

  Serial.print("Sleeping for ");
  Serial.println(seconds_to_sleep);

  return seconds_to_sleep;
}