//Include Particle Device OS APIs
#include "Particle.h"

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
#define PUBLISHING 0

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
const unsigned long SECONDS_BETWEEN_MEASUREMENTS = 360; // What should sampling period be?
const unsigned long EARLYBIRD_SECONDS = 0; // how long before desired time should I wake up? 
const unsigned long TIMEOUT_TINYCAM_MS = 5000;

String statement;

void setup() {

  if (PUBLISHING == 1) {
    Particle.connect();
  } else {
    Cellular.off(); // turn off cellular for prelim testing (uncomment)
  }

  Serial.begin(9600);
  Serial1.begin(9600); // Initialize serial communication
  pinMode(A0, OUTPUT); 
  Serial1.setTimeout(TIMEOUT_TINYCAM_MS);
}

void loop() {
  // Enter state machine
  switch (state) {
  case DATALOG_STATE: {
    Serial1.flush();
    delay(100);
    Serial1.begin(9600);
    delay(100);
    digitalWrite(A0, LOW);
    delay(1000);
    digitalWrite(A0, HIGH);
    digitalWrite(A0, LOW);
    delay(2000);
    if(Serial1.available() > 0)
    {
      statement = Serial1.readString();
    } else {
        Serial.print("could not receive from openmv"); 
      }

    Serial.println(statement); 
    real_time = Time.now();

    Serial1.end();
    Serial1.flush(); 
    delay(100);
    Serial1.begin(9600);
    delay(1000); 
    if(Serial1.available() > 0)
    {
      statement = Serial1.println(real_time);
    } else {
      Serial.print("could not send datetime to openmv"); 
    }

    delay(2000);
    
    // Print out data buffer
    Serial.print(real_time);
    Serial.print(",");
    Serial.println(statement); 
    Serial1.end();

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

    while (!isMaxTime) {
      //connect particle to the cloud
      if (Particle.connected() == false) {
        Particle.connect();
        Serial.print("Trying to connect");
      }

      // If connected, publish data buffer
      if (Particle.connected()) {

        Serial.println("publishing data");
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
    //int wakeInSeconds = secondsUntilNextEvent(); // Calculate how long to sleep 
    int wakeInSeconds = 30;

    config.mode(SystemSleepMode::ULTRA_LOW_POWER)
      .gpio(D2, FALLING) //Is this supposed to be D2 pin?? ************
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
