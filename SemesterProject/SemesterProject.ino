// Includes //
#include <LiquidCrystal.h>
#include <DHT.h>
#include <IRremote.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#define DHTPIN 6
#define DHTTYPE DHT11
#define RDA 0x80
#define TBE 0x20

//---------------------- Variables ---------------------------//
//---------------- Water Level Variables ---------------------//
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
volatile unsigned char* my_DIDR0 = (unsigned char*) 0x7E;
volatile unsigned char* my_DDRF = (unsigned char*) 0x30;
//------------------------------------------------------------//

//------------------ Humidity & Temp variables --------------//
DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
uint8_t readButton(void);
float h;
float f;
//-----------------------------------------------------------//

//---------------------- LCD Variable -----------------------//
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//-----------------------------------------------------------//

//---------------------- Motor Variables --------------------//
bool Motor_Direction = true;
volatile unsigned char* motorAddress = &PORTA;
int receiver = 13;
IRrecv irrecv(receiver);
decode_results results;
int directions[] = {0b01000000, 0b01100000, 0b00100000, 0b00110000, 0b00010000, 0b00011000, 0b00001000, 0b01001000};
//-----------------------------------------------------------//

//----------------------- LED Setup ------------------------//
volatile unsigned char* ledArray = &PORTC;
int red = 0b10000000;
int yellow = 0b01000000;
int green = 0b00100000;
int blue = 0b00010000;
int lights[] = {yellow, green, red, blue};
//----------------------------------------------------------//

//---------------------- Fan Setup -------------------------//
volatile unsigned char* fanPort = &PORTL;
int on = 0b10000000;
int off = 0b00000000;
//----------------------------------------------------------//

//--------------------- Logic Variables --------------------//
int State = 0;
int LastState = 0;
float temp_Threshold = 72;
String SN[] = {"Disabled", "Idle", "Error", "Running"};
//----------------------------------------------------------//

//--------------------- End Variables ----------------------//

void setup() {
  Serial.begin(9600);
  while (!Serial);
  dht.begin();
  lcd.begin(16, 2); 
  lcd.print("Temp:  Humidity:");
  adc_init();
  irrecv.enableIRIn();
  DDRA = 0b01111000;        // Setting step motor pins to output 
  DDRC = 0b11110000;        // Setting LED pins to output
  DDRL = 0b10000000;
  *ledArray = lights[State];
}

void loop() {
  Remote_Read();
  switch(State)
  {
    case 0:
      break;
      
    case 1:
      Display();
      if(Temp_Check()) break;
      if(Water_Check()) break;
      break;
      
    case 2:
      if(Water_Check()) break;
      break;
      
    case 3:
      Display();
      if(Temp_Check()) break;
      if(Water_Check()) break;
      break;
  }

  if (LastState != State){
    Display();
    Fan();
    Record_Transition();
    LastState = State;
  }
}

void Record_Transition(){
  tmElements_t tm;
  String Line;
  Line = "State changed from: " + SN[LastState] + " to: " + SN[State] + " on: ";
  Serial.print(Line);
  if (RTC.read(tm)){
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.write(" at: ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.println(); 
  }
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void Remote_Read(){
  if(irrecv.decode(&results)){
    switch(results.value)
    {
      case 0xFFE01F:    // Up Button pressed
        Motor_Direction = true;
        Change_Angle();
        break;
        
      case 0xFF906F:    // Down Button pressed
        Motor_Direction = false;
        Change_Angle();
        break;

      case 0xFFA857:    // Volume Down Button pressed
        temp_Threshold -= 1;
        break;
          
      case 0xFF629D:    // Volume Up Button pressed
        temp_Threshold += 1;
        break;

      case 0xFFA25D:    // Power Button pressed
        if(State == 0){
          State = 1;
          *ledArray = lights[State];
          break; 
        }
        else{
          State = 0;
          *ledArray = lights[State];
        }
        break;
    }  
    irrecv.resume();
  }
}

void Fan(){
  if(State == 3){
    *fanPort = on;         //Turn on fan
  }
  else{
    *fanPort = off;  //Turn off fan
  }
}

void Display(){
  if(State == 0){
    lcd.setCursor(0, 1); 
    lcd.print("            ");
  }
  else if(State == 1 || State == 3){
    lcd.setCursor(0, 1);
    h = dht.readHumidity();
    f = dht.readTemperature(true);
    
    lcd.print(f);
    lcd.setCursor(7, 1);
    lcd.print(h);
  }
  else{
    lcd.setCursor(0, 1);
    lcd.print("Error       ");
  } 
}

void Change_Angle(){
  if(Motor_Direction){
    for (int i = 0; i < 129; i++){
      for (int j = 0; j < 7; j++){
       *motorAddress = directions[j];
        delay(3); 
      }
      *motorAddress = directions[7];
      delay(3);
    }
  }
  else{
    for (int i = 0; i < 129; i++){
      for (int j = 6; j > -1; j--){
       *motorAddress = directions[j];
        delay(3);
      }
      *motorAddress = directions[7];
      delay(3);
    }
  }
  *motorAddress = 0;
}

bool Temp_Check(){
  bool change = false;
  if(State == 1 && dht.readTemperature(true) > temp_Threshold){
    State = 3;
    *ledArray = lights[State];
    Fan();
    change = true;
  }
  else if(State == 3 && dht.readTemperature(true) < temp_Threshold){
    State = 1;
    *ledArray = lights[State];
    Fan();
    change = true;
  }
  return change;
}

bool Water_Check(){
  bool change = false;
  if((State == 1 || State == 3) && adc_read() < 450){
    State = 2;
    *ledArray = lights[State];
    change = true;
  }
  else if(State == 2 && adc_read() > 450){
    State = 1;
    *ledArray = lights[State];
    change = true;
  }
  return change;
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
  
unsigned int adc_read()
{  
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX &= 0xE0;

  // clear the channel selection bits (MUX 5) //MUX 5 is in the ADCSRB register
  *my_ADCSRB &= 0x77;
  
  // set the channel selection bits
  *my_ADMUX = *my_ADMUX | 0b01000001;
  
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  
  // wait for the conversion to complete
  while (( * my_ADCSRA & 0x40) != 0); 

  // return the result in the ADC data register
  return *my_ADC_DATA; 
}
