/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);


// setup() runs once, when the device is first turned on
void setup() {
  Cellular.off(); 
  Serial1.begin(9600);
  Serial.begin(9600); 
  // Put initialization like pinMode and begin functions here
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  
  String statement = Serial1.readString();
  Serial.println(statement); 

  Serial1.end(); 

  // while (Serial1.available()==0) {
  //   if( ++timeout > 10000){ // set this to your timeout value in milliseconds
  //   //error handling code here
  //   break;
  //   }
  // }
  //timeout = 0; // got a char so reset timeout
  delay(2000); 

  Serial1.begin(9600); 
  
  long real_time = Time.now(); 
  Serial.println(real_time); 
  Serial1.print(real_time); //I want it to send its datetime to the openmv

  delay(20000); 
}
