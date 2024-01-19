
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

SystemSleepConfiguration config;

// Various timing constants
//const unsigned long MAX_TIME_TO_PUBLISH_MS = 20000; // Only stay awake for this time trying to connect to the cloud and publish
// const unsigned long TIME_AFTER_PUBLISH_MS = 4000; // After publish, wait 4 seconds for data to go out
const unsigned long SECONDS_BETWEEN_MEASUREMENTS = 60; // What should sampling period be?
const unsigned long EARLYBIRD_SECONDS = 0; // how long before desired time should I wake up? 

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600); // Initialize serial communication
    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH);

}

void loop() {

        digitalWrite(A0, LOW);

        delay(1000);

        //Serial.println(Serial1.readString());

        //delay(3000);

        digitalWrite(A0, HIGH);

        digitalWrite(A0, LOW);

        delay(1000);

        Serial.println(Serial1.readString());

        //delay(3000);

        digitalWrite(A0, HIGH);

         config.mode(SystemSleepMode::ULTRA_LOW_POWER)
         .gpio(A0, FALLING)
         .duration(60* 1000L); // Set seconds until wake


//Ready to sleep
        SystemSleepResult result = System.sleep(config); // Device sleeps here
     //}

     //int secondsUntilNextEvent() {

         //int current_seconds = Time.now();
         //int seconds_to_sleep = SECONDS_BETWEEN_MEASUREMENTS - EARLYBIRD_SECONDS; //- (current_seconds % SECONDS_BETWEEN_MEASUREMENTS);

         //Serial.print("Sleeping for ");
         //Serial.println(seconds_to_sleep);

         //return seconds_to_sleep;
}
