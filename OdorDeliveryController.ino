/*
 * 
 * Sketch for controlling odor delivery setup



 MIT License

Copyright (c) 2026 Wriju Mitra and Kamakshi Singh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 * 
 * 
 * 28.08.26 Fixed error(Air valve was not open by default)
 * Alternative is to keep the air line free of any valves(probably better less ideal as we want the final volume to remain constant)
 * 
 * SDA-> A4 
  SCL-> A5 
  Working
  28.08.26
 */






#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);


int PotentiometerPin =A0;
int Valve1 = 13;
int Valve2 = 12;
int Valve3 =  11;
int buzzerPin = 10;
int ValveSelect1 = 9;
int ValveSelect2 = 8;

int interruptPin =2;


unsigned long StimDur;

unsigned long StartTime;


volatile bool State = 0;

static unsigned long lastInterruptTime = 0;



void setup()                                            //Switch on air asap
{
 pinMode(Valve1, OUTPUT);
 pinMode(Valve2, OUTPUT);
 pinMode(Valve3, OUTPUT);                             // Solenoid for Air
 pinMode(buzzerPin, OUTPUT);
 digitalWrite(Valve1, HIGH);
 digitalWrite(Valve2, HIGH);
 digitalWrite(Valve3, LOW);

 digitalWrite(buzzerPin, LOW);

 pinMode(ValveSelect1, INPUT_PULLUP);
 pinMode(ValveSelect2, INPUT_PULLUP);

 pinMode(interruptPin, INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(interruptPin), Trigger, LOW);

 lcd.init();
 delay(100);
 lcd.backlight();
 WelcomeMessage();



  
}



void loop() 
{
 Display();         //Display Stimulation Time and Odor ID

 StimDur= CalculateDur()*1000;              //Get Value from potentiometer. Multiply by 1000 to convert to ms
 


 if(State)
 {
  digitalWrite(buzzerPin, HIGH);

  lcd.setCursor(0,0);
  lcd.print("Stimulus ****");

  unsigned long StartTime = millis();

  int countDown;


  while((millis()-StartTime)<StimDur && State)
  { 
     digitalWrite(Valve3, HIGH);   //Switch off air
    countDown = (StimDur - (millis()-StartTime))/1000;
    
    lcd.setCursor(0,1);
    lcd.print("Time Left: ");
    lcd.print(countDown);
    lcd.print("s  ");

    if(digitalRead(ValveSelect1)==LOW)
       digitalWrite(Valve1, LOW);
    else if(digitalRead(ValveSelect2)==LOW)
       digitalWrite(Valve2, LOW);
    else                                       //No odor selected
      {
        lcd.setCursor(0,1);  
        lcd.print("ErrorCode1");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Select Odor");  
        lcd.setCursor(0,1); 
        lcd.print("and try again!");
        delay(3000);
        State=0;
        break;
      }

          
  
  }

  digitalWrite(Valve3, LOW);   //Switch air back on again 
  lcd.clear();
   
 }

 State = 0;
 digitalWrite(buzzerPin, LOW);
 digitalWrite(Valve1, HIGH);  
 digitalWrite(Valve2, HIGH);          //Close both valves

 
 //lcd.clear();

 delay(300); 
  
}





unsigned long CalculateDur()                       //function returns Stimulus duration
{
  
  unsigned long t=  map(analogRead(PotentiometerPin), 0, 1023, 300, 0);
  return t;
}



void WelcomeMessage()
{

String message1 = "Odor Delivery Setup for in-vivo imaging                ";
String message2 = "https://github.com/Wriju-Mitra/                ";
String message3 = "MIT License Copyright (c) 2026 Wriju & Kamakshi                ";


lcd.setCursor(0,0);
lcd.print("HELLO"); // first row statis
 delay(200);
 
//scrolling within the second row only
 for(int i = 0; i < message1.length(); i++) 
 {

 lcd.setCursor(0,1);
 lcd.print(message1.substring(i,i+16));
 delay(200);
}
 lcd.clear();



for(int i = 0; i < message2.length(); i++) 
 {
 //lcd.clear();
 lcd.setCursor(0,0);
 lcd.print(message2.substring(i,i+16));
 
 lcd.setCursor(0,1);
 lcd.print(message3.substring(i,i+16));
 delay(200);
}
 lcd.clear();



 //Print Welcome Message
}



void Display()
{
 
 lcd.setCursor(0,0);
 lcd.print("Stim Dur=");
 lcd.print(CalculateDur());
 lcd.setCursor(0,1);
 
 if(digitalRead(ValveSelect1)==LOW)
        lcd.print("Odor1          ");
 else if(digitalRead(ValveSelect2)==LOW)
         lcd.print("Odor2         ");
 else
        lcd.print("Select Odor");        
 

   
}




void Trigger()
{

  if(millis() - lastInterruptTime < 50)
    return;            //do nothing as this is probably a bounce


  else
  {
  State=!State;
  lastInterruptTime = millis();
  }

}
