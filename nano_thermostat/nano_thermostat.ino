/*
Things to add (to-do list):
    Heat Pump - Done
    A/C
    Fan
    Heater
    Split out into multiple functions/files
    LCD Readout
    Function to switch modes


*/

/* VAR SETS */
int MODE = 4; // MODE 0-4; 0 - disabled; 1 - heater; 2 - fan; 3 - A/C; 4 - heat pump
int SET_TEMP = 68;      // Low temp at which the heat turns on (60*F)
int ALARM_TEMP = 40;   // Critical Low temp (40*F)
int LPDELAY = 30; // Seconds to delay each loop
int PUMP_IDLE = 0; // Count for Heat Pump Functionality, gotta keep those pipes warm!
/* VAR SETS */

/* PIN ASSIGNMENT */
//Alarms
int ALARM_PIN = A6;      // Alarm for low temp
int HEAT_LED = 2;        // Pin for heat "on" LED
int TEMP_OK = 3;        // LED for when temp is above threshold
int ALARM_LED = 4;      // Alarm LED 

int HEAT_PIN = A1;      // Relay For Heater
int FAN_PIN = A2;       // Relay For Fan
int AC_PIN = A3;        // Relay For AC
int HEATPUMP_PIN = A4;  // Relay For Heat Pump
/* PIN ASSIGNMENT */


#include "DHT.h"

#define DHTPIN A0
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)



// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

// Readings used for average
const int numReadings = 3;

// Set variables to zero
float avetemp = 0;                
float temp = 0;
float checkdelay = 0;           

void setup() {
  
  Serial.begin(9600); 
  Serial.println("Temp Monitor Started.\n");

  dht.begin();
  pinMode(DHTPIN,INPUT);
  pinMode(ALARM_PIN,OUTPUT);
  pinMode(ALARM_LED,OUTPUT);
  pinMode(HEAT_LED,OUTPUT);
  pinMode(TEMP_OK,OUTPUT);
  
  pinMode(HEATPUMP_PIN,OUTPUT);
  digitalWrite(HEATPUMP_PIN, HIGH);
  
  Serial.println("Temp set to ");
  Serial.println(SET_TEMP);
  Serial.println("Initializing complete, Waiting 3 seconds.");
  delay(3000);
 }



void loop() {
  
  temp = 0;
  Serial.print("Realtime Temp: \t");
    
  for (int x = 0; x < numReadings; x++){
    float f = dht.readTemperature(true);            // Read temperature as Fahrenheit
    Serial.print(f);
    Serial.print("\t");
    temp = f + temp;
    delay(5000);                                    // delay in between reads for stability  
  }    
  
  Serial.println();
  avetemp = temp / numReadings;                    // calculate the average
  Serial.print("Average Temp is ");
  Serial.println(avetemp);                         // send it to the computer as ASCII digits
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!\n");
    return;
  }

/*
  // Read serial input
  int tmp_temp = Serial.parseInt(); //read int or parseFloat for ..float...
  if ( tmp_temp != SET_TEMP ){
    SET_TEMP = tmp_temp;
    Serial.print("Temp Changed to: ");
    Serial.println(SET_TEMP);
  }//end if
*/

  // Heat Pump Mode
  if ( MODE == 4 ){
    if ( avetemp < ALARM_TEMP ) {
      Serial.print( "Critical temp returned. Is your sensor returning correct value?\n" );
      digitalWrite(ALARM_LED, HIGH);
    } else {
      digitalWrite(ALARM_LED, LOW);
    } //end if/else   
   
   //Pump control function
   if ( avetemp < SET_TEMP ){
     Serial.print( "Turning on Heat Pump.\n" );
     digitalWrite(HEATPUMP_PIN, LOW);
     PUMP_IDLE = 0;
   } else {
     PUMP_IDLE = PUMP_IDLE + 1;
     Serial.print( "Temp is above set, nothing to do.\n" );
     digitalWrite(HEATPUMP_PIN, HIGH);
   }//end if/else
    
    //This function will add efficiency to a heat pump system. Just because the room gets warmer doesnt mean that the floor isn't losing temp.
    if ( PUMP_IDLE >= 180000 / LPDELAY && avetemp <= SET_TEMP + 15 ){
      Serial.print( "Cycling Heat Pump.\n" );
      delay(60000);
    }//end if
    
  }//end if

  // Wait x seconds before recheck.. this may be too long or to short?
  delay(LPDELAY);
}
