//Settings

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2

#define DHTTYPE DH22

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

float tempSet = 0;
int angleSet = 0;

// Variables in system
float temp = 0, humidity = 0;
int lastState = 0, state = 0, waterLevel = 0, vAngle;


void setup() {
	Serial.begin(9600);
	dht.begin();
	sensor_t sensor;
	dht.temperature().getSensor(&sensor);
	Serial.println(F("------------------------------------");
	Serial.println(F("Temperature Sensor"));
	Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
	Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
	Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
	Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
	Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
	Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
	Serial.println(F("------------------------------------"));
	dht.humidity().getSensor(&sensor);
	Serial.println(F("Humidity Sensor"));
	Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
	Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
	Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
	Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
	Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
	Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
	Serial.println(F("------------------------------------"));
	delayMS = sensor.min_delay / 1000;
}

void loop() {
  //Setup Led indication.
  LEDS();
  Vent();

  //Check for state change and save info if changed.
  if(lastState != state){
    Save();
	
  delay(delayMS);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
    
  //Idle State
  if(state == 1){
    //Check for state changes due to these conditions
    Temp();
    Display();
    WaterLevel();
  }
  
  //Running State
  else if(state == 2){
    //Check for state changes due to these conditions
    Temp();
    Motor();
    Display();
    WaterLevel();
  }
  
  //Error State
  else if(state == 3){
    //Check for state changes due to these conditions
    Motor();
    Display();
    WaterLevel();
  }
  
  //Default: Disabled State
  else{
    //Check for on/off switch change
    Motor();
    Display();
  }
}

void Temp(){
  // Check temp and change state accordingly
  temp = //some reading we will need to get.
  if(state == 1){
    if(tempSet < temp){
      state = 2; 
    }
  }
  else{
    if(tempSet >= temp){
      state = 1;
    }
  }
  // Check humidity
  humidity = //some reading we will need to get.
}

void WaterLevel(){
  // Check water level
  waterLevel = //some reading we will need to get.
  float minLevel = //some number we will need to decide on.
  if(waterLevel < minLevel){
    state = 3;
  }
}

void Motor(){
  if(lastState != state){
    if(state == 3){
      // turn motor off
      
    }
    else{
      // turn motor on
      
    }
  }
}

void Display(){
  if(state == 3){
    // Print Error msg and water level to screen.
    
  }
  else{
    // Print Temp and humidity to screen.
    
  }
}

void LEDS(){
  if(lastState != state){
    if(state == 1){
      // Light the Green LED.
      
    }
    else if(state == 2){
      // Light the Blue LED.
      
    }
    else if(state == 3){
      // Light the Red LED.
      
    }
    else{
      // Light the Yellow LED.
      
    }
  }
}

void Vent(){
  //check for change in address to see if angle has changed vAngle and angleSet might need to be hex values.
  // Set vent angle
  if(vAngle != angleSet){
    //Change vent angle somehow.
  }
  angleSet = vAngle;
}

void Save(){
  // Save information on the state change with file writing.
  lastState = state;
}
