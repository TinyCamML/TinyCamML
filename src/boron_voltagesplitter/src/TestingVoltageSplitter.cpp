/* 
 * Project TestingVoltageSplitter
 * Author: Liz Farquhar
 * Date: 4/9/2024
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);



// setup() runs once, when the device is first turned on
void setup() {
  Cellular.off(); 
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  float voltage = analogRead(A1) * ((3.3/4096)*((2000000+1300000)/2000000));
  // 3.3V / 4096 counts * ((R1 + R2) / R1) where R1 and R2 are in ohms
  delay(100); 
  Serial.println(voltage); 
  delay(1000); 
}