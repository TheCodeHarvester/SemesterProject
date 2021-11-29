// Includes //
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 6
#define DHTTYPE DHT11

//                    Variables                               //
//------adc read definitions For water level -----------------//
#define RDA 0x80
#define TBE 0x20  
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
volatile unsigned char* my_DIDR0 = (unsigned char*) 0x7E;
volatile unsigned char* my_DDRF = (unsigned char*) 0x30;
unsigned int waterLevel = 0;
//------------------------------------------------------------//

//-------------------Humidity & Temp variables --------------//
DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
uint8_t readButton(void);
float h;
float f;
//-----------------------------------------------------------//

//-----------------------LCD Variable------------------------//
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//-----------------------------------------------------------//

//---------------------- Motor Variables --------------------//
bool Motor_Direction = true;
volatile unsigned char* motorAddress = &PORTA;
//-----------------------------------------------------------//

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2); 
  lcd.print("Temp:  Humidity:");
  adc_init();
  DDRA = B01111000;
}

void loop() {
  delay(delayMS);
  waterLevel = adc_read(0);
  Display();
  // Just testing that the angle changes both directions
  Motor_Direction = true;
  Change_Angle();
  Motor_Direction = false;
  Change_Angle();
}

void Display(){
  lcd.setCursor(0, 1);
  h = dht.readHumidity();
  f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(f)) {
	  lcd.print("Error!");
	  return;
  }
  
  lcd.print(f);
  lcd.setCursor(7, 1);
  lcd.print(h);
}

void Change_Angle(){
  if(Motor_Direction){
    for (int i = 0; i < 129; i++){
      *motorAddress = B01000000;
      delay(5);
      *motorAddress = B01100000;
      delay(5);
      *motorAddress = B00100000;
      delay(5);
      *motorAddress = B00110000;
      delay(5);
      *motorAddress = B00010000;
      delay(5);
      *motorAddress = B00011000;
      delay(5);
      *motorAddress = B00001000;
      delay(5);
      *motorAddress = B01001000;
      delay(5); 
    }
  }
  else{
    for (int i = 0; i < 129; i++){
      *motorAddress = B00001000;
      delay(5);
      *motorAddress = B00011000;
      delay(5);
      *motorAddress = B00010000;
      delay(5); 
      *motorAddress = B00110000;
      delay(5);
      *motorAddress = B00100000;
      delay(5);
      *motorAddress = B01100000;
      delay(5);
      *motorAddress = B01000000;
      delay(5);
      *motorAddress = B01001000;
      delay(5);
    }
  }
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
  unsigned int channel_nmbr = 0xE0;
  
  // set the channel selection bits
  *my_ADMUX &= channel_nmbr;
  
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;

  // wait for the conversion to complete
  while (( * my_ADCSRA & 0x40) != 0); 

  // return the result in the ADC data register
  return *my_ADC_DATA; 
}
