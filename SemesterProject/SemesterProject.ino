//Settings

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2

#define DHTTYPE DH22

//adc read definitions
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
volatile unsigned char* my_DIDR0 = (unsigned char*) 0x7E;
volatile unsigned char* my_DDRF = (unsigned char*) 0x30;

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
uint8_t readButton(void);

float tempSet = 0;
int angleSet = 0;

int waterSensor = A0; //water level sensor pin

// Variables in system
float temp = 0, humidity = 0;
int lastState = 0, state = 0, waterLevel = 0, vAngle;

void setup() {
	Serial.begin(9600);
	
	DDRD &= ~(); // configure input for power button
	PORTD |= (); //activate pullups
	DDRB |= (); //configure outputs
		
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
	adc_init();
}

void loop() {
  //Setup Led indication.
  LEDS();
  Vent();

  // loop to verify power button state
  while(1) {
	  if (readButton()==1) {
		  PORTB ^=(); // XOR to toggle
	  }
	  _delay_ms(250);
  }
  
  // debounce read value to verify button press is valid
  uint8_t readButton(void) {
	  if ((PIND & (1<<)) == 0) {
		  _delay_ms(25);
	  }
  if ((PIND & (1<<)) == 0) {
	  return 1;
  }
  else {
	  return 0;
  }

  
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
  // Check water level; written to ADC[0]
  waterLevel = adcRead(0); //read as analog
  if(waterLevel < 100){
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
    Serial.print  (F("Warning: Water level too low!"));
  }
  else{
    // Print Temp and humidity to screen.
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
	
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%")); 
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
	
void adc_init()
{
   // setup the A register
  *my_ADCSRA |= 0x85;
  *my_ADCSRA &= 0x87;
  // setup the B register
  *my_ADCSRB &= (0x01 << 6);
  // setup the MUX Register
  *my_ADMUX |= (0x01 << 7); //sets MSB bit to 1, REFS1
  *my_ADMUX &= 0x9C; //sets REFS0 and ADLAR for right justification
  //disabling digital input for all pins
  *my_DIDR0 |= 0x81; //disables all digital input pins for buffer
}
	
unsigned int adc_read(unsigned char adc_channel_num)
{  
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX &= 0xE0;

  // clear the channel selection bits (MUX 5) //MUX 5 is in the ADCSRB register
  *my_ADCSRB &= 0x77;
  
  // set the channel number
  unsigned int channel_nmbr = channels[adc_channel_num];
  
  // set the channel selection bits
  *my_ADMUX &= channel_nmbr;
  
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;

  // wait for the conversion to complete
  while (( * my_ADCSRA & 0x40) != 0); 

  // return the result in the ADC data register
  return *my_ADC_DATA; 
}
