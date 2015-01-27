/*Arduino Thermometer
      by electricmango
  http://electricmango.github.io
      based on the work of bildr.org
            from http://bildr.org/2011/07/ds18b20-arduino

  ----------------

  English Blog post:
  http://electronicmango.blogspot.kr/2014/02/arduino-thermometer.html

  한국어 글:
  http://blog.naver.com/mangodaniel/187384726

  YouTube video:
  http://youtu.be/68qfwJzcAPI
*/

#include <OneWire.h>
#include <LiquidCrystal.h>

int DS18B20_Pin = 6; //DS18S20 Signal pin on digital 6

//Temperature chip i/o
OneWire ds(DS18B20_Pin);  // on digital pin 6
LiquidCrystal lcd (12,11,10,9,8,7);
int select = 2;

byte data[12];
byte addr[8];

int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers


void setup(void){
  lcd.begin(16,2);
  pinMode(select, INPUT);
  digitalWrite(select, HIGH);
}

void loop(void){
  lcd.clear();
  reading = digitalRead(select);
  if(reading == HIGH && previous == LOW && millis() - time > debounce){
    if(state == HIGH)
      state = LOW;
    else
      state = HIGH;
    time = millis();    
  }
  previous = reading;
  float temperature = getTemp();
  if(!ds.search(addr)){
      lcd.clear();
      //no more sensors on chain, reset search
      lcd.print("Error 0: No reply from sensor"); 
      delay(1000);
      for(int positionCounter = 0; positionCounter < 13; positionCounter++){
        lcd.scrollDisplayLeft(); 
        delay(300);
      }
      delay(1000);
      lcd.clear();
      ds.reset_search();
      lcd.print("Searching");
      delay(1000);
      lcd.print(".");
      delay(333);
      lcd.print(".");
      delay(333);
      lcd.print(".");
      delay(333);
      lcd.clear();
      ds.reset_search();
  }
  if(OneWire::crc8( addr,7) != addr[7]){
      lcd.clear();
      lcd.print("Error 1: CRC is not valid");
      delay(1000);
      for(int positionCounter = 0; positionCounter < 9; positionCounter++){
        lcd.scrollDisplayLeft(); 
        delay(300);
      }
      delay(1000);
  }
  if(addr[0] != 0x10 && addr[0] != 0x28){
      lcd.clear();
      lcd.print("Error 2: Device is not recognized");
      delay(1000);
      for(int positionCounter = 0; positionCounter < 17; positionCounter++){
        lcd.scrollDisplayLeft(); 
        delay(300);
      }
      delay(1000);
  }
  lcd.print(temperature);
  lcd.setCursor(6,0);
  if(state == HIGH){
    lcd.print((char)223);
    lcd.print("Celcius");
    lcd.setCursor(0,1);
    lcd.print(temperature * 9 / 5 + 32);
    lcd.setCursor(6,1);
    lcd.print((char)223);
    lcd.print("Farenheit");
  }
  else{
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print(temperature * 9 / 5 + 32);
    lcd.setCursor(6,1);
    lcd.print((char)223);
    lcd.print("F");
  }
  delay(100); //just here to slow down the output so it is easier to read
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  ds.reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  return TemperatureSum;
}
