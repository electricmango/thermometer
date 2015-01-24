/*Arduino Thermometer
      by Electricmango (망고)
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

  ----------------

  The MIT License (MIT)

  Copyright (c) 2013 ~ 2015 electricmango

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  
  ----------------
  
  version notes
  
  1.0.2
    - license and useless stuff changes.
    
  1.0.1
    - the code is now on GitHub (but removed from GitHub Gist):
      https://github.com/electricmango/Thermometer under the MIT license!
      However, there won't be any updates from me for a while for this code
      because I fried my DS18B20 sensor.
    
  1.0.0
    - Released to Github Gist. 
  0.4.b0
    - Select mode (Celcius, Farenheit or C, F)
  0.3.b0
   - Farenheit support
   
  0.2.a1
   - Spaces for better look
   
  0.2.a0
   - User friendly debug mode
   
  0.1.a1
   - Added degree mark
   - Better debug mode
   
  0.1.0
   - Thanks to bildr.org for the DS18S20 code
   - Added lcd compatibility to the DS18S20 code
   
  Thanks to 
    Bildr.org for the serial code
    http://www.arduino.cc/en/Tutorial/Switch for mode selecting code
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
